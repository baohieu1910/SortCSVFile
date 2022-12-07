#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

#define FILE_NAME "Books_rating.csv" // Large file name
#define OUTPUT_FILE_NAME "sorted_books_rating.csv" // Output file name
#define SORTED_SMALL_FILE "output_Books_rating_" // Sorted small file name
#define FULL_SIZE 100000 // The desired lines of small files are divided

struct List // Manage each small file
{
    ifstream input_file;
    int size = 0;      // Number of elements imported from the file
    int full_size = 0; // Maximum number of elements imported from the file
    bool check = true; // The status of the element to continues to input or not, True is allowed
    string s = "";     // The string will be transmitted in each file
};

// Merge vector contains string
void merge_vector(vector<string> &list, int const left, int const mid, int const right)
{
    int const sub1 = mid - left + 1;
    int const sub2 = right - mid;
    vector<string> subLeft;
    vector<string> subRight;
    for (int i = 0; i < sub1; i++)
    {
        subLeft.push_back(list[left + i]);
    }
    for (int i = 0; i < sub2; i++)
    {
        subRight.push_back(list[mid + i + 1]);
    }
    int indexSub1 = 0;
    int indexSub2 = 0;
    int indexMerge = left;
    while (indexSub1 < sub1 && indexSub2 < sub2)
    {
        if (subLeft[indexSub1].substr(0, 10).compare(subRight[indexSub2].substr(0, 10)) <= 0)
        {
            list[indexMerge] = subLeft[indexSub1];
            indexSub1++;
        }
        else
        {
            list[indexMerge] = subRight[indexSub2];
            indexSub2++;
        }
        indexMerge++;
    }
    while (indexSub1 < sub1)
    {
        list[indexMerge] = subLeft[indexSub1];
        indexSub1++;
        indexMerge++;
    }
    while (indexSub2 < sub2)
    {
        list[indexMerge] = subRight[indexSub2];
        indexSub2++;
        indexMerge++;
    }
}

//Sort vector contains string
void merge_sort_vector(vector<string> &list, int const begin, int const end)
{
    if (begin >= end)
        return;
    int mid = begin + (end - begin) / 2;
    merge_sort_vector(list, begin, mid);
    merge_sort_vector(list, mid + 1, end);
    merge_vector(list, begin, mid, end);
}

// Split large file to multi small file and sorting
void create_and_sort_small_file(int &SIZE, int &LINE)
{
    fstream input_file, output_file;
    int count = 0;
    int count_2 = 0;
    input_file.open(FILE_NAME, ios::in | ios::binary);
    string s;
    while (getline(input_file, s))
    {
        vector<string> temp; // vector used to import string and sorting in vector
        output_file.open(SORTED_SMALL_FILE + to_string(count + 1) + ".csv", ios::out | ios::binary);
        temp.push_back(s);
        count_2++;
        int x = 0;

        while (getline(input_file, s))
        {
            temp.push_back(s);
            count_2++;
            if (x >= FULL_SIZE - 2)
                break;
            x++;
        }
        if (count == 0) // Sorting all line in file except line 1 (Id,Title,Price,User_id, ..........)
        {
            merge_sort_vector(temp, 1, temp.size() - 1);
        }
        else
        {
            merge_sort_vector(temp, 0, temp.size() - 1);
        }
        for (int i = 0; i < temp.size(); i++)
        {
            output_file << temp[i] << endl;
        }
        output_file.close(); // Close file that opened
        count++;
        cout << "Created output_Books_rating_" + to_string(count) << endl;
    }
    input_file.close();
    SIZE = count;   // Numbers of small file
    LINE = count_2; // Line numbers of large file
}

void merge_multi_file(int SIZE, int LINE)
{
    List *list = new List[SIZE];
    ofstream output_file;

    for (int i = 0; i < SIZE; i++)
    {
        list[i].input_file.open(SORTED_SMALL_FILE + to_string(i + 1) + ".csv", ios::in | ios::binary);
        if (i == SIZE - 1)
        {
            list[i].full_size = LINE - (SIZE - 1) * FULL_SIZE;
        }
        else
        {
            list[i].full_size = FULL_SIZE;
        }
    }
    output_file.open(OUTPUT_FILE_NAME, ios::out | ios::binary);
    int count = 0;
    while (count < LINE)
    {
        for (int i = 0; i < SIZE; i++)
        {
            if (list[i].size < list[i].full_size && list[i].check == true)
            {
                getline(list[i].input_file, list[i].s);
            }
        }
        string min = "zzzzzzzzzz";
        int temp = 0; // Position file have min value
        for (int i = 0; i < SIZE; i++)
        {
            if (list[i].size < list[i].full_size)
            {
                if (min.compare(list[i].s.substr(0, 10)) > 0)
                {
                    min = list[i].s;
                    temp = i;
                }
            }
        }
        if (count == 0)
        {
            if (list[0].size == 0)
            {
                output_file << list[0].s << endl;
                getline(list[0].input_file, list[0].s);
                list[0].size++;
            }
        }
        else
        {
            output_file << list[temp].s << endl;
            list[temp].size++;
        }
        for (int i = 0; i < SIZE; i++)
        {
            if (i == temp)
            {
                list[i].check = true;
            }
            else
            {
                list[i].check = false;
            }
        }
        count++;
        if(count % 500000 == 0 || count == LINE){
            cout << count << " lines have been sorted!!!" << endl;
        }
        }
    for (int i = 0; i < SIZE; i++)
    {
        list[i].input_file.close();
    }
    output_file.close();
    delete[] list;
}

// Remove all small file was created
void remove_file(int SIZE)
{
    for (int i = 0; i < SIZE; i++)
    {
        string s = SORTED_SMALL_FILE + to_string(i + 1) + ".csv";
        remove(s.c_str());
    }
}
int main()
{
    int size = 0;
    int line = 0;
    cout << "Creating and sorting small file..." << endl;
    create_and_sort_small_file(size, line);
    cout << "Merging multi sorted small file into large file..." << endl;
    merge_multi_file(size, line);
    cout << "Finish sorting large file!!!" << endl;
    cout << "Removing all small File..." << endl;
    remove_file(size);
    cout << "Removed all small File!!!" << endl;
    cout << "Final sorted file name is: sorted_books_rating.csv" << endl;
    return 0;
}
