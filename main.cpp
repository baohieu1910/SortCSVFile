
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

#define FILE_NAME "Books_rating.csv"
#define FULL_SIZE 100000 // số dòng mong muốn của các file nhỏ được chia

struct List // quản lí từng file nhỏ
{
    ifstream input_file;
    int size = 0;      // số lượng các phần tử được nhập từ file
    int full_size = 0; // số lượng tối ta các phần tử có thể nhập được từ file
    bool check = true; // trạng thái của phần tử xem có tiếp tục nhập vào hay không, true là được phép nhập false là không
    string s = "";     // chuỗi sẽ được truyền vào ở mỗi file
};
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

void merge_sort_vector(vector<string> &list, int const begin, int const end)
{
    if (begin >= end)
        return;
    int mid = begin + (end - begin) / 2;
    merge_sort_vector(list, begin, mid);
    merge_sort_vector(list, mid + 1, end);
    merge_vector(list, begin, mid, end);
}


void create_and_sort_small_file(int &SIZE, int &LINE)
{
    fstream input_file, output_file;
    int count = 0;
    int count_2 = 0;
    input_file.open(FILE_NAME, ios::in | ios::binary);
    string s;
    while (getline(input_file, s))
    {
        vector<string> temp; // vector dùng để truyền phần tử vào và sắp xếp sau đó xuất ra file nhỏ
        output_file.open("output_Books_rating_" + to_string(count + 1) + ".csv", ios::out | ios::binary);
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
        if (count == 0) // sắp xếp file trừ dòng đầu tiền là dòng định nghĩa (Id,Title,Price,User_id, ..........)
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
        output_file.close(); // đóng file đã mở
        count++;
        cout << "Created File_" + to_string(count) << endl;
    }
    input_file.close();
    SIZE = count;   // số file nhỏ được chia ra
    LINE = count_2; // tổng số dòng của file lớn
}

void merge_multi_file(int SIZE, int LINE)
{
    List *list = new List[SIZE];
    ofstream output_file;

    for (int i = 0; i < SIZE; i++)
    {
        list[i].input_file.open("output_Books_rating_" + to_string(i + 1) + ".csv", ios::in | ios::binary);
        if (i == SIZE - 1)
        {
            list[i].full_size = LINE - (SIZE - 1) * FULL_SIZE;
        }
        else
        {
            list[i].full_size = FULL_SIZE;
        }
    }
    output_file.open("sorted_books_rating.csv", ios::out | ios::binary);
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
        int temp = 0;
        // vị trí file chứa phần tử nhỏ nhất
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
        cout << count << endl;
        }
    for (int i = 0; i < SIZE; i++)
    {
        list[i].input_file.close();
    }
    output_file.close();
    delete[] list;
}

void remove_file(int SIZE)
{
    for (int i = 0; i < SIZE; i++)
    {
        string s = "output_Books_rating_" + to_string(i + 1) + ".csv";
        remove(s.c_str()); // remove tất cả các file nhỏ
    }
}
int main()
{
    int size = 0;
    int line = 0;
    create_and_sort_small_file(size, line);
    cout << "Created" << endl;
    cout << "Sorting" << endl;
    merge_multi_file(size, line);
    cout << "Sorted" << endl;
    cout << "Removing Small File" << endl;
    remove_file(size);
    cout << "DONE" << endl;
    return 0;
}
