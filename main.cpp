#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <set>

using namespace std;

struct fact
{
    string from_unit;
    double conversion_factor;
    string to_unit;
};

struct query
{
    string from_unit;
    double measurement;
    string to_unit;
};

struct vertex
{
    double prev_edge;
    string unit;
    unordered_map<string, double> convertable_units;
};

void facts_to_graph(const vector<fact>&,
        unordered_map<string, unordered_map<string, double>>&);

void convert_measurement(const double&, double&, unordered_map<string, unordered_map<string, double>>&, const query& query);

int main() {

    unordered_map<string, unordered_map<string, double>> adjacency_list;
    
    vector<fact> facts {
        {
            {"m", 3.28, "ft"},
            {"ft", 12, "in"},
            {"min", 60, "sec"},
            {"hr", 60, "min"},
            {"in", 2.54, "cm"}
        }
    };

    vector<query> queries {
        {
            {"m", 100, "ft"},
            {"ft", 1, "in"},
            {"min", 1, "sec"},
            {"hr", 2, "min"},
            {"hr", 2, "sec"},
            {"hr", 2, "m"},
            {"cm", 21234, "ft"}
        }
    };

    facts_to_graph(facts, adjacency_list);

    for(query query : queries) {

        cout << "Query: " << query.measurement << " " << query.from_unit
            << " to " << query.to_unit << endl;

        double original_measurement = query.measurement;
        double converted_measurement = query.measurement;

        try {
            convert_measurement(original_measurement, converted_measurement, adjacency_list, query);

            cout << "Converted Measurement: " << converted_measurement << " " << query.to_unit
                << " derived from " << original_measurement << " " << query.from_unit << endl;
        } catch (exception e) {
            cout << "Error: could not convert measurement" << endl;
        }
    }

    return 0;
}

void insert_inverse_edge(unordered_map<string, unordered_map<string, double>>& adjacency_list, const fact& fact) {

    unordered_map<string, unordered_map<string, double>>::iterator fetched_fact = adjacency_list.find(fact.from_unit);

    if(fetched_fact == adjacency_list.end()) {
        unordered_map<string, double> conversion_factors;
        conversion_factors.insert({fact.to_unit, fact.conversion_factor});
        adjacency_list.insert({fact.from_unit, conversion_factors});
    } else {
        unordered_map<string, double> &conversion_factors = fetched_fact->second;
        conversion_factors.insert({fact.to_unit, fact.conversion_factor});
    }
}

void insert_edge(unordered_map<string, unordered_map<string, double>>& adjacency_list, const fact& fact) {

    unordered_map<string, unordered_map<string, double>>::iterator fetched_fact = adjacency_list.find(fact.from_unit);

    if(fetched_fact == adjacency_list.end()) {
        unordered_map<string, double> conversion_factors;
        conversion_factors.insert({fact.to_unit, fact.conversion_factor});
        adjacency_list.insert({fact.from_unit, conversion_factors});
    } else {
        unordered_map<string, double> &conversion_factors = fetched_fact->second;
        conversion_factors.insert({fact.to_unit, fact.conversion_factor});
    }

    insert_inverse_edge(adjacency_list, {fact.to_unit, (1 / fact.conversion_factor), fact.from_unit});
    
}

void facts_to_graph(const vector<fact>& facts,
        unordered_map<string, unordered_map<string, double>>& adjacency_list) {
    
    for(fact fact : facts) {
        insert_edge(adjacency_list, fact);
    }

}

void convert_measurement(const double& original_measurement, double& converted_measurement, unordered_map<string, unordered_map<string, double>>& adjacency_list, const query& query) {

    queue<vertex> to_visit;
    unordered_map<string, vertex> visited;
    unordered_map<string, unordered_map<string, double>>::iterator found = adjacency_list.find(query.from_unit);

    to_visit.push({1, found->first, found->second});
    vertex v = to_visit.front();

    while(!to_visit.empty()) {
        v = to_visit.front();
        converted_measurement *= v.prev_edge;
        to_visit.pop();
        visited.insert({v.unit, v});
        if(v.unit == query.to_unit) {
            return;
        }
        unordered_map<string, double>::iterator conversion = v.convertable_units.begin();
        while(conversion != v.convertable_units.end()) {
            if(visited.find(conversion->first) == visited.end()) {
                to_visit.push({conversion->second, adjacency_list.find(conversion->first)->first, adjacency_list.find(conversion->first)->second});
            }
            conversion++;
        }
    }
    throw exception();
}