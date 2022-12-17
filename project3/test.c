struct Worker
{
    int id;
    int salary;
    int group;
};

struct Group
{
    int group_id;
    struct Worker workers[10];
    int avg_salary;
};

int main()
{
    struct Group company[5];
    company[2].workers[3].salary = 1;
    // int a[2][3];
    // a[1][2] = a[2][1];
}
