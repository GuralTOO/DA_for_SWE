#include<bits/stdc++.h>

using namespace std;

struct NewGradApplicant{
	map<string, int> attributes;
	map<string, double> preferences;
	vector<pair<double, int>> preferenceList;	//second value in pair contains the index of position
	priority_queue <pair<double, int>, vector<pair<double, int>>> preferenceQueue;
	string status;
	int curMatch = -1;
	
	NewGradApplicant(){
		//attributes
		attributes["university_ranking"] = rand()%1000;
		attributes["GPA"] = rand()%1000;
		attributes["internship_count"] = rand()%1000;

		//preferences	
		double pref_salary = rand()%100+1;
		double pref_perks = rand()%100+1;
		double pref_sum = pref_salary + pref_perks;
		preferences["salary"] = pref_salary/pref_sum;
		preferences["perks"] = pref_perks/pref_sum;

		//misc
		status = "unmatched";
	}
};

struct Position{
	map<string, int> attributes;	
	map<string, double> preferences;	
	vector<pair<double, int>> preferenceList;	//second value in pair contains the index of applicant
	priority_queue <pair<double, int>, vector<pair<double, int>>> preferenceQueue;
	map<int, int> applicantRank;
	string status;
	int curMatch = -1;
	
	Position(){
		//attributes
		attributes["industry"] = rand()%5+1;	//fixed
		attributes["salary"] = rand()%1000;
		attributes["location"] = rand()%10+1;	//fixed
		attributes["perks"] = rand()%1000;
	
		//preferences
		double pref_university_ranking = rand()%100+1;
		double pref_GPA = rand()%100+1;
		double pref_internship_count = rand()%100+1;
		double pref_sum = pref_university_ranking + pref_GPA + pref_internship_count;
		preferences["internship_count"] = pref_internship_count/pref_sum;
		preferences["GPA"] = pref_GPA/pref_sum;
		preferences["university_ranking"] = pref_university_ranking/pref_sum;
		
		//misc
		status = "unmatched";
	}
};

vector<NewGradApplicant> applicants;
vector<Position> positions;
int numApplicants, numPositions;

void build(){
	
	//generate applicants
	for(int i=0; i<numApplicants; i++)
		applicants.push_back(NewGradApplicant());

	//generate positions
	for(int i=0; i<numPositions; i++)
		positions.push_back(Position());		

	//generate applicants' preferenceLists
	for(auto & applicant: applicants){
		for(int j=0; j<numPositions; j++){
			pair<double, int> tempPair = {0, j};
			for(auto preference: applicant.preferences)
				tempPair.first += double(positions[j].attributes[preference.first] * preference.second);
			applicant.preferenceList.push_back(tempPair);
			applicant.preferenceQueue.push(tempPair);
		}
//		cout<<applicant.preferenceQueue.size()<<endl;
		sort(applicant.preferenceList.begin(), applicant.preferenceList.end());
		reverse(applicant.preferenceList.begin(), applicant.preferenceList.end());
	}
	
	//generate positions' preferenceLists
	for(auto & position: positions){
		for(int j=0; j<numApplicants; j++){
			pair<double, int> tempPair = {0, j};
			for(auto preference: position.preferences)
				tempPair.first += double(applicants[j].attributes[preference.first] * preference.second);
			position.preferenceList.push_back(tempPair);
			position.preferenceQueue.push({tempPair});
		}
		sort(position.preferenceList.begin(), position.preferenceList.end());
		reverse(position.preferenceList.begin(), position.preferenceList.end());
		for(int i=0; i<position.preferenceList.size(); i++)
			position.applicantRank[position.preferenceList[i].second] = i;
	}
	
}

int runOneRound(){
	pair<double, int> tempPair;
	int i = -1;
//	cout<<"Running a new round"<<endl;
	for(auto & applicant: applicants){
		i++;
		//skip if the applicant is currently matched or ran out matches
		if(applicant.status == "matched" or applicant.preferenceQueue.empty())
			continue;
//		cout<<applicant.status<<endl;
		//take their top preference that they haven't approached			
		tempPair = applicant.preferenceQueue.top();
		applicant.preferenceQueue.pop();

		//approach them 1) check if they are unmatched 2)check if this applicant is more prefereble to their cur match
		if(positions[tempPair.second].status == "unmatched"){
			positions[tempPair.second].status = "matched";
			positions[tempPair.second].curMatch = i;
			applicant.status = "matched";
			applicant.curMatch = tempPair.second;
//			cout<<"New Match - "<<i<<" : "<<tempPair.second<<endl;
		}
		else if(positions[tempPair.second].applicantRank[positions[tempPair.second].curMatch] > positions[tempPair.second].applicantRank[i]){
			//remove matching from the position's previous match
			applicants[positions[tempPair.second].curMatch].status = "unmatched";
			applicants[positions[tempPair.second].curMatch].curMatch = -1;
			positions[tempPair.second].curMatch = i;
			applicant.status = "matched";
			applicant.curMatch = tempPair.second;
//			cout<<"Match changed - "<<i<<" : "<<tempPair.second<<endl;
		}
	}
	int stillRunning = 0;
	for(auto applicant: applicants){
//		cout<<applicant.status<<" "<<applicant.curMatch<<endl;
		if(applicant.status == "unmatched" && !applicant.preferenceQueue.empty())
			stillRunning++;		
	}
	return stillRunning;	
}

void work(){

	//run an applicant proposing DA until all candidates are matched, or they run out of options.
	int matches = 0;
	while(runOneRound()){
//		cout<<"Running another round"<<endl;
	}
	for(auto applicant: applicants)
		matches += (applicant.status == "matched");
	cout<<"Matches: "<<matches<<endl;
	
}

int main(){
	srand((unsigned) time(NULL));
	cout<<"Enter the number of applicants: "<<endl;
	cin>>numApplicants;
	cout<<"Enter the number of positions: "<<endl;
	cin>>numPositions;
	
	build();
	work();
	
	return 0;
}
