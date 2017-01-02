#include <cstdio>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iomanip>
#include <limits>
using namespace std;

vector<string> input_weather;
vector<bool> input_yes_no;//true == yes;false == no
int input_size = 0;

const long double weather_transition_matrix[3][3] = {{0.8l,0.15l,0.05l},
                                          {0.2l,0.5l,0.3l},
                                          {0.2l,0.2l,0.6l}};
const long double coat_weather_prediction_matrix[2][3] = {{0.9l,0.7l,0.2l},
                                               {0.1l,0.3l,0.8l}};

const long double weather_start_prob[3] = {0.5l,0.25l,0.25l};


vector<vector<long double>> daily_probs_all;
vector<vector<long double>> calculated_probs_all;

long double find_max_probs(vector<long double> *probs,int idx)
{
    long double max = -1.0;
    assert(idx < 3);
    for(int i = idx*3;i < idx*3+3;i++)
    {
        if((*probs)[i] > max)
        {
            max = (*probs)[i];
        }
    }
    return max;
}
int find_max_probs_idx(vector<long double> *probs)//for backtracking
{
    long double max = -1;
    int idx = 0;
    for(int i = 0;i < 3;i++)
    {
        if((*probs)[i] > max)
        {
            max = (*probs)[i];
            idx = i;
        }
    }
    return idx;
}

void viterbi_solver(FILE *output_file)
{
    vector<long double> probs;//sun,fog,rain
    vector<long double> calc_probs;// sun_sun,sun_fog,sun_rain,etc(9 possibilities)
    int idx;
    long double result,coat_prob;
    if(!input_yes_no[0])//coat == no
    {
        idx = 0;
    }else//coat == yes
    {
        idx = 1;
    }
    for(int i = 0;i < 3;i++)//initial probabilities
    {
        probs.push_back(weather_start_prob[i]*coat_weather_prediction_matrix[idx][i]);
    }
    daily_probs_all.push_back(probs);
    probs.clear();
    //main algorithm
    for(int i = 0;i < input_size-1;i++)
    {
        for(int j = 0;j < 3;j++)
        {
            for(int k = 0;k < 3;k++)
            {
                result = daily_probs_all[i][k]*weather_transition_matrix[k][j];
                calc_probs.push_back(result);
            }
            if(!input_yes_no[i+1])//no
            {
                coat_prob = coat_weather_prediction_matrix[0][j];
            }else//yes
            {
                coat_prob = coat_weather_prediction_matrix[1][j];
            }
            result = find_max_probs(&calc_probs,j)*coat_prob;
            probs.push_back(result);
        }
        calculated_probs_all.push_back(calc_probs);
        calc_probs.clear();
        daily_probs_all.push_back(probs);
        probs.clear();
    }
    //backtracking for getting predicted result
    long double max_val = -1.0;
    int max_idx = 0;
    vector<int> result_weather_num;
    vector<string> result_weather_string;
    idx = find_max_probs_idx(&daily_probs_all[input_size-1]);
    result_weather_num.insert(result_weather_num.begin(),idx);
    for(int j = input_size-1;j > 0;j--)
    {
        max_idx = -1;
        max_val = -1.0;
        cout << "max(";
        for(int i =idx*3; i < idx*3+3;i++)
        {
            cout << calculated_probs_all[j-1][i] << ",";
            if(calculated_probs_all[j-1][i] >= max_val)
            {
                max_val = calculated_probs_all[j-1][i];
                max_idx = i-idx*3;
            }
        }
        cout << ")" << endl;
        result_weather_num.insert(result_weather_num.begin(),max_idx);
        idx = max_idx;
    }
    for(uint32_t i = 0;i < result_weather_num.size();i++)
    {
        switch(result_weather_num[i])
        {
            case 0:
                result_weather_string.push_back("sunny");
                break;
            case 1:
                result_weather_string.push_back("foggy");
                break;
            case 2:
                result_weather_string.push_back("rainy");
                break;
            default:
                printf("ERROR\n");
                exit(EXIT_FAILURE);
        }
    }
    int count = 0;
    for(uint32_t i = 0;i < result_weather_string.size();i++)
    {
        if(input_weather[i] == result_weather_string[i])
        {
            count++;
        }
    }
    fprintf(output_file,"%g\n",((double)count)/((double)input_size));
    for(int i = 0;i < input_size;i++)
    {
        //cout << result_weather_string[i] << endl;
        fprintf(output_file,"%s\n",result_weather_string[i].c_str());
    }
}

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

void parse_input(char *file)
{
    ifstream infile(file);
    string s;
    while (infile >> s)
    {
        //cout << s << endl;
        std::istringstream iss(s);
        std::string token;
        while (std::getline(iss, token, ','))
        {
            //std::cout << token << std::endl;
            switch(str2int(token.c_str()) )
            {
                case str2int("sunny"):
                case str2int("foggy"):
                case str2int("rainy"):
                    input_weather.push_back(token);
                    break;
                case str2int("yes"):
                    input_yes_no.push_back(true);
                    break;
                case str2int("no"):
                    input_yes_no.push_back(false);
                    break;
                default:
	                //do nothing
                    break;
            }
        }
    }
    input_size = input_weather.size();
/*
    cout << input_size << endl;
    for(int i = 0;i < input_size;i++)
    {
        cout << input_weather[i] << ",";
        if(input_yes_no[i])
        {
            cout << "yes" << endl;
        }else
        {
            cout << "no" << endl;
        }
    }
*/
}

int main(int argc,char **argv)
{
    std::setprecision(std::numeric_limits<long double>::digits10);
    FILE *input_file = fopen(argv[1],"r");
    FILE *output_file = fopen(argv[2],"w");
    if(!input_file || !output_file)
    {
        printf("open %s or %s fail!\n",argv[1],argv[2]);
    }
    parse_input(argv[1]);
    viterbi_solver(output_file);

    fclose(input_file);
    return 0;
}
