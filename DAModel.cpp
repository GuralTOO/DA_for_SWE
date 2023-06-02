#include <bits/stdc++.h>
#define numInterviews 5

struct Applicant {
    int id;
    double gpa;
    int internship_experience;
    int university_ranking;
    double normalized_gpa;
    double normalized_internship_experience;
    double normalized_university_ranking;
    std::bitset<10> skills;
    std::vector<double> pref_weights;
    std::vector<std::pair<int, double>> ranked_companies;
};

struct Company {
    int id;
    double salary;
    double perks;
    int company_size;
    double normalized_salary;
    double normalized_perks;
    double normalized_company_size;
    std::bitset<10> skill_requirements;
    std::vector<double> pref_weights;
    std::vector<std::pair<int, double>> ranked_applicants;
};

double normalize(double value, double min_value, double max_value, double min_range, double max_range) {
    return min_range + ((value - min_value) * (max_range - min_range)) / (max_value - min_value);
}

struct Match {
    int applicant_id;
    int company_id;
};

double compute_utility(const Applicant& applicant, const Company& company) {
    double utility = applicant.pref_weights[0] * company.normalized_salary +
           			 applicant.pref_weights[1] * company.normalized_perks +
           			 applicant.pref_weights[2] * company.normalized_company_size;
    utility += company.pref_weights[0] * applicant.normalized_gpa +
               company.pref_weights[1] * applicant.normalized_internship_experience +
               company.pref_weights[2] * applicant.normalized_university_ranking;

	return utility;
}

bool is_compatible(const Applicant& applicant, const Company& company) {
	return (applicant.skills & company.skill_requirements) == company.skill_requirements;
}

void create_ordered_list_of_applicants(Company& company, const std::vector<Applicant>& applicants) {
    for (const Applicant& applicant : applicants) {
        if (company.skill_requirements.to_ulong() & applicant.skills.to_ulong() == company.skill_requirements.to_ulong()) {
            double utility = company.pref_weights[0] * applicant.normalized_gpa +
                             company.pref_weights[1] * applicant.normalized_internship_experience +
                             company.pref_weights[2] * applicant.normalized_university_ranking;
            company.ranked_applicants.push_back({applicant.id, utility});
        }
    }
	std::sort(company.ranked_applicants.begin(), company.ranked_applicants.end(),
              [](const std::pair<int, double>& a, const std::pair<int, double>& b) { return a.second > b.second; });
}

void create_ordered_list_of_companies(Applicant& applicant, const std::vector<Company>& companies) {
    for (const Company& company : companies) {
        if ((company.skill_requirements & applicant.skills) == company.skill_requirements) {
            double utility = applicant.pref_weights[0] * company.normalized_salary +
                             applicant.pref_weights[1] * company.normalized_perks +
                             applicant.pref_weights[2] * company.normalized_company_size;
            applicant.ranked_companies.push_back({company.id, utility});
        }
    }

    std::sort(applicant.ranked_companies.begin(), applicant.ranked_companies.end(),
              [](const std::pair<int, double>& a, const std::pair<int, double>& b) { return a.second > b.second; });
}

std::vector<Match> modified_deferred_acceptance_algorithm(const std::vector<Applicant>& applicants, const std::vector<Company>& companies, int num_interviews) {
	std::vector<Match> matches;
	std::vector<std::queue<int>> applicant_queues(applicants.size());
	std::vector<std::vector<int>> company_slots(companies.size(), std::vector<int>());

	// Create a compatibility matrix
	std::vector<std::vector<int>> compatibility_matrix(applicants.size(), std::vector<int>(companies.size(), 0));

	for (int i = 0; i < applicants.size(); ++i) {
	    for (int j = 0; j < companies.size(); ++j) {
	        compatibility_matrix[i][j] = is_compatible(applicants[i], companies[j]) ? 1 : 0;
	    }
	}
	
	
	// Initialize applicant queues with compatible companies
	for (int i = 0; i < applicants.size(); ++i) {
	    for (std::pair<int, double> j : applicants[i].ranked_companies) {
	        if (compatibility_matrix[i][j.first]) {
	            applicant_queues[i].push(j.first);
	        }
	    }
	}
	
	
	
	while (true) {
	    bool progress = false;
	
	    for (int a = 0; a < applicants.size(); ++a) {
	        if (!applicant_queues[a].empty()){
				int c = applicant_queues[a].front();
				applicant_queues[a].pop();
	
	            if (company_slots[c].size() < num_interviews) {
	                company_slots[c].push_back(a);
	                progress = true;
	            } else {
	                int lowest_ranked_applicant = -1;
	                int lowest_rank = -1;
	
	                for (std::pair<int, double> ranked_applicant : companies[c].ranked_applicants) {
	                    auto it = std::find(company_slots[c].begin(), company_slots[c].end(), ranked_applicant.first);
	                    if (it != company_slots[c].end()) {
	                        int rank = std::distance(companies[c].ranked_applicants.begin(), std::find(companies[c].ranked_applicants.begin(), companies[c].ranked_applicants.end(), ranked_applicant));
	                        if (lowest_rank == -1 || rank > lowest_rank) {
	                            lowest_rank = rank;
	                            lowest_ranked_applicant = ranked_applicant.first;
	                        }
	                    }
	                }
					std::pair<int, double>a_prime = {a, 0};
	                int a_rank = std::distance(companies[c].ranked_applicants.begin(), std::find(companies[c].ranked_applicants.begin(), companies[c].ranked_applicants.end(), a_prime));
	                if (a_rank < lowest_rank) {
	                    company_slots[c].erase(std::find(company_slots[c].begin(), company_slots[c].end(), lowest_ranked_applicant));
	                    company_slots[c].push_back(a);
	                    applicant_queues[lowest_ranked_applicant].push(c);
	                    progress = true;
	                } else {
	                    applicant_queues[a].push(c);
	                }
	            }
	        }
	    }

	    if (!progress) {
	        break;
	    }
	}

	for (int c = 0; c < companies.size(); ++c){
		for (int a : company_slots[c]) {
			matches.push_back({a, c});
		}
	}

	return matches;
}



int main() {
	// Generate applicants and companies using a normal distribution for the attributes
	int num_applicants = 10000;
	int num_companies = 5000;
	int num_interviews = numInterviews;
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<double> gpa_distribution(3.5, 0.5);
	std::normal_distribution<double> internship_distribution(2, 1);
	std::normal_distribution<double> ranking_distribution(150, 50);
	std::normal_distribution<double> salary_distribution(100000, 20000);
	std::normal_distribution<double> perks_distribution(1000, 200);
	std::normal_distribution<double> size_distribution(500, 100);
	
	std::vector<Applicant> applicants(num_applicants);
	std::vector<Company> companies(num_companies);
	
	for (int i = 0; i < num_applicants; ++i) {
	    applicants[i] = Applicant{
	        i,
	        std::min(std::max(gpa_distribution(gen), 0.0), 4.0),
	        std::min(std::max((int)internship_distribution(gen), 0), 5),
	        std::min(std::max((int)ranking_distribution(gen), 1), 200),
	        0,
	        0,
	        0,
	        std::bitset<10>(rd() % 1024),
	        {0.5, 0.35, 0.15},
	        {}
	    };
	}
	

	for (int i = 0; i < num_companies; ++i) {
		companies[i] = {
			i,
			std::min(std::max(salary_distribution(gen), 50000.0), 200000.0),
			std::min(std::max(perks_distribution(gen), 0.0), 5000.0),
			std::min(std::max((int)size_distribution(gen), 10), 2000),
			0,
			0,
			0,
			std::bitset<10>(rd() % 1024),
			{0.5, 0.35, 0.15},
			{}
		};
	}
	
	for (Applicant& applicant : applicants) {
        applicant.normalized_gpa = normalize(applicant.gpa, 0.0, 4.0, 0, 10);
        applicant.normalized_internship_experience = normalize(applicant.internship_experience, 0, 5, 0, 10);
        applicant.normalized_university_ranking = normalize(applicant.university_ranking, 1, 200, 0, 10);
    }
    
    for (Company& company : companies) {
        company.normalized_salary = normalize(company.salary, 50000, 200000, 0, 10);
        company.normalized_perks = normalize(company.perks, 0, 5000, 0, 10);
        company.normalized_company_size = normalize(company.company_size, 10, 2000, 0, 10);
    }

	
	// Create ordered ranking lists for applicants and companies
	for (Applicant& applicant : applicants) 
	    create_ordered_list_of_companies(applicant, companies);
	
	for (Company& company : companies) 
	    create_ordered_list_of_applicants(company, applicants);
	
	// Run the modified deferred acceptance algorithm
	std::vector<Match> matches = modified_deferred_acceptance_algorithm(applicants, companies, num_interviews);    

	
	// Compute resulting total utility
	double total_utility = 0;
	int applicants_matched = 0, companies_matched = 0;	
	std::vector<int> applicant_is_matched(num_applicants, 0);
	std::vector<int> company_is_matched(companies.size(), 0);

	for (const Match& match : matches) {
	    total_utility += compute_utility(applicants[match.applicant_id], companies[match.company_id]);
	    if(!applicant_is_matched[match.applicant_id]) {
	    	applicant_is_matched[match.applicant_id] = 1;
	    	applicants_matched ++;
	    }
	    if(!company_is_matched[match.company_id]) {
	    	company_is_matched[match.company_id] = 1;
	    	companies_matched ++;
	    }
	}
	
	std::cout << "Number of applicnats in the pool: " << num_applicants << std::endl;
	std::cout << "Number of companies in the pool: " << num_companies << std::endl;
	std::cout << "Number of interview matches accepted by each company: " << num_interviews << std::endl;
	
	
	std::cout << "Total utility: " << total_utility << std::endl;
	std::cout << "Companies matched: " << companies_matched << " out of " << num_companies << std::endl;
	std::cout << "Applicants matched: " << applicants_matched << " out of " << num_applicants << std::endl;
	

	//	Computing maximum achievable total utility.
	std::priority_queue<std::pair<double, std::pair<int, int>>, std::vector<std::pair<double, std::pair<int, int>>>> potential_matches;

	
	// Initialize a queue of potential matches in the decreasing order of utility
	std::vector <int> capacity_company(num_companies, num_interviews);
	for (const Applicant& applicant : applicants){
		for(auto temp_val: applicant.ranked_companies){
			int company_id = temp_val.first;
			potential_matches.push({compute_utility(applicant, companies[company_id]), {applicant.id, company_id}});
		}
	}
	
		
	double total_achievable_utility = 0;
	while(!potential_matches.empty()) {
		
		auto temp_val = potential_matches.top();
		potential_matches.pop();

		int company_id = temp_val.second.second;
		int applicant_id = temp_val.second.first;
		double match_utility = temp_val.first;
		
		if(capacity_company[company_id]){
			total_achievable_utility += match_utility;
			capacity_company[company_id] --;	
		}
	}
	std::cout << "Maximum achievable utility: " << total_achievable_utility << std::endl;
	
	return 0;
}
