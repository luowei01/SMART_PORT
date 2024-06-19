#include <bits/stdc++.h>
using namespace std;
void logError(const std::string &message)
{
    std::ofstream logfile("error.log", std::ios_base::app);
    if (logfile.is_open())
    {
        logfile << message << std::endl;
        logfile.close();
    }
    else
    {
        std::cerr << "Unable to open log file" << std::endl;
    }
}
const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;
const int boat_num = 5;
int sum_value = 0;
int sum_num = 0;
int money, boat_capacity, id;
char ch[N][N];
struct Good
{
    int x, y, val, die_id;
    bool is_disappeared=false;//是否被拿走而在地图上消失
    Good(int x, int y, int val, int die_id){
        this->x = x;
        this->y = y;
        this->val = val;
        this->die_id = die_id;
    }
};
vector<Good *> goods;
struct Robot
{
    int x, y, goods;
    int status;
    Good *goal_good = nullptr;
    int goal_berth_id = -1;
    vector<pair<int, int>> path = {}; // 路径记录
    Robot() {}
    Robot(int startX, int startY)
    {
        x = startX;
        y = startY;
    }
    void reset()
    {
        path = {};
    }
}robot[robot_num];
Good *findNearestGood(int roubot_id)
{
    double minDist = numeric_limits<double>::max();
    Good *nearestItem;
    for (const auto &good : goods)
    {
        if (good->is_disappeared || id>good->die_id)
            continue;
        double dist = sqrt(pow(good->x - robot[roubot_id].x, 2) + pow(good->y - robot[roubot_id].y, 2));
        if (dist < minDist)
        {
            minDist = dist;
            nearestItem = good;
        }
    }
    return nearestItem;
}
struct Berth
{
    int x;
    int y;
    int id;
    vector<int> berth_goods;
    int transport_time;
    int loading_speed;
    Berth(){}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
    }
}berth[berth_num];
struct Boat
{
    int num, pos, status;
    int from = -1;
    int loading_time=0;
}boat[boat_num];
int findNearestBerth(int roubot_id)
{
    double minDist = numeric_limits<double>::max();
    Berth nearestItem;
    for (const auto &item : berth)
    {
        double dist = sqrt(pow(item.x - robot[roubot_id].x, 2) + pow(item.y - robot[roubot_id].y, 2));
        if (dist < minDist)
        {
            minDist = dist;
            nearestItem = item;
        }
    }
    return nearestItem.id;
}

void Init()
{
    for (int i = 0; i < n; i++)
        scanf("%s", ch[i]);
    for(int i = 0; i < berth_num; i ++)
    {
        scanf("%d", &id);
        berth[id].id = id;
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
    }
    scanf("%d", &boat_capacity);
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

int Input()
{
    scanf("%d%d", &id, &money);
    int num;
    scanf("%d", &num);
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
        Good* good = new Good{x, y, val, id+1000};
        goods.push_back(good);
    }
    for(int i = 0; i < robot_num; i ++)
    {
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &robot[i].status);
    }
    for(int i = 0; i < 5; i ++)
        scanf("%d%d\n", &boat[i].status, &boat[i].pos);
    char okk[100];
    scanf("%s", okk);
    return id;
}
struct Direction
{
    int dx;
    int dy;
    std::string name;
    int name_id;
};
Direction directions[4] = {
    {0, 1, "Right", 0},
    {0, -1, "Left", 1},
    {-1, 0, "Up", 2},
    {1, 0, "Down", 3}};
int getDirectionName_id(int robot_id,pair<int,int> next){
    int dx = next.first - robot[robot_id].x;
    int dy = next.second - robot[robot_id].y;
    if(dx==0 && dy==0)return -1;
    if(dx==0){
        if(dy==1) return 0;
        else if(dy==-1) return 1;
        else return -2;
    }
    else if(dy==0){
        if(dx==-1) return 2;
        else if(dx==1) return 3;
        else return -2;
    }
    return -1;
}
//定义节点结构体
struct Node
{
    int x, y;
    int g, h;
    Node *parent;

    Node(int _x, int _y, int _g, int _h, Node *_parent)
        : x(_x), y(_y), g(_g), h(_h), parent(_parent) {}

    int f() const
    {
        return g + h;
    }
};
// 定义比较函数，用于优先队列
struct CompareNode
{
    bool operator()(const Node *a, const Node *b) const
    {
        return a->f() > b->f();
    }
};
// A* 算法
vector<pair<int, int>> AStar(pair<int, int> start, pair<int, int> end)
{
    vector<pair<int, int>> path={};
    priority_queue<Node *, vector<Node *>, CompareNode> openSet;
    vector<vector<bool>> closedSet(n, vector<bool>(n, false));
    vector<vector<Node *>> nodes(n, vector<Node *>(n, nullptr));

    Node *startNode = new Node(start.first, start.second, 0, 0, nullptr);
    openSet.push(startNode);
    nodes[start.first][start.second] = startNode;
    while (!openSet.empty())
    {
        Node *current = openSet.top();
        openSet.pop();
        if (current->x == end.first && current->y == end.second)
        {
            while (current != nullptr)
            {
                path.push_back({current->x, current->y});
                current = current->parent;
            }
            break;
        }
        closedSet[current->x][current->y] = true;
        for (const Direction &dir : directions)
        {
            int nx = current->x + dir.dx;
            int ny = current->y + dir.dy;
            if (nx < 0 || nx >= n || ny < 0 || ny >= n){
                continue;
            }
            if (ch[nx][ny] == '*' || ch[nx][ny] == '#' || closedSet[nx][ny])
            {
                continue;
            }
            int ng = current->g + 1;
            int nh = abs(nx - end.first) + abs(ny - end.second);

            if (nodes[nx][ny] == nullptr)
            {
                Node *neighbor = new Node(nx, ny, ng, nh, current);
                openSet.push(neighbor);
                nodes[nx][ny] = neighbor;
            }
            else if (ng < nodes[nx][ny]->g)
            {
                nodes[nx][ny]->g = ng;
                nodes[nx][ny]->parent = current;
            }
        }
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            delete nodes[i][j];
        }
    }

    return path;
}
vector<thread> threads;
mutex mtx;
const int MAX_THREADS = 4;
condition_variable cv;
int running_threads = 0;
// 多线程搜索函数
void searchThread(pair<int, int> start, pair<int, int> end,mutex &mtx, int robot_id)
{
    vector<pair<int, int>> path = AStar(start, end);
    // Output result
    unique_lock<mutex> lock(mtx);
    if(path.size()>0)path.pop_back(); // remove start point
    robot[robot_id].path = path;
    running_threads--;
    // Notify waiting threads
    cv.notify_all();
}
void makeDestination()
{
    // 检查是否会与其他机器人相交
    unordered_map<string, vector<int>> positionsMap;
    for (int i = 0; i < robot_num; i++)
    {
        if(robot[i].path.empty()) {
            continue;
        }
        string key = to_string(robot[i].path.back().first) + "," + to_string(robot[i].path.back().second);
        positionsMap[key].push_back(i);
    }
    //随机选一个可以移动的机器人
    for (auto it = positionsMap.begin(); it != positionsMap.end(); ++it)
    {
        if(it->second.size() == 2){
            int robot1 = it->second[0];
            int robot2 = it->second[1];
            if(robot[robot1].x==robot[robot2].x )//对撞，修正路线绕开
            {
                if (ch[robot[robot1].x - 1][robot[robot1].y] == '.' || ch[robot[robot1].x - 1][robot[robot1].y] == 'B')
                {
                    robot[robot1].path.push_back({robot[robot1].x-1, robot[robot1].y});
                    robot[robot1].path.push_back({robot[robot2].x-1, robot[robot2].y});
                }
                else
                {
                    robot[robot1].path.push_back({robot[robot1].x + 1, robot[robot1].y});
                    robot[robot1].path.push_back({robot[robot2].x + 1, robot[robot2].y});
                }
            }
            else if (robot[robot1].y == robot[robot2].y)
            {
                if (ch[robot[robot1].x][robot[robot1].y-1] == '.' || ch[robot[robot1].x][robot[robot1].y-1] == 'B')
                {
                    robot[robot1].path.push_back({robot[robot1].x, robot[robot1].y-1});
                    robot[robot1].path.push_back({robot[robot2].x, robot[robot2].y-1});
                }
                else
                {
                    robot[robot1].path.push_back({robot[robot1].x, robot[robot1].y+1});
                    robot[robot1].path.push_back({robot[robot2].x, robot[robot2].y+1});
                }
            }
            else //错位相撞，等待对方通过
            {
                int rand_i = it->second[rand() % it->second.size()];
                robot[rand_i].path.push_back({robot[rand_i].x, robot[rand_i].y});
            }
        }
    }

    for (int i = 0; i < robot_num; i++)
    {
        if (robot[i].status == 0){
            // if (robot[i].goods == 0)
            // {
            //     if (robot[i].goal_good != nullptr){
            //         goods.push_back(robot[i].goal_good);
            //         robot[i].goal_good = nullptr;
            //     }                   
            // }
            // else
            // {
            //     robot[i].goal_berth_id == -1; 
            // }
            // robot[i].reset(); // 重置机器人状态
            continue;               
        }              
        if (robot[i].goods == 0)// 未携带货物
        {
            if (robot[i].path.empty()&&robot[i].goal_good == nullptr&&robot[i].goal_berth_id == -1) // 无货物目标,找一个最近的货物
            {
                pair<int, int> start = {robot[i].x, robot[i].y};
                Good* good = findNearestGood(i);
                pair<int, int> end = {good->x,good->y}; 
                robot[i].goal_good = good;
                goods.erase(std::remove(goods.begin(), goods.end(), good), goods.end()); // 删掉该货物,独享
                threads.emplace_back(searchThread, start, end, ref(mtx), i);
            }
            else if (!robot[i].path.empty() && robot[i].goal_good != nullptr && robot[i].goal_berth_id == -1) // 在前往货物的路上
            {
                // 检查机器人状态，不能取到货物的机器人应该放弃任务并释放
                if (robot[i].goal_good->is_disappeared || id + robot[i].path.size() > robot[i].goal_good->die_id){
                    goods.push_back(robot[i].goal_good);
                    robot[i].reset();
                    robot[i].goal_good = nullptr;
                }
                else{
                    auto next = robot[i].path.back();
                    robot[i].path.pop_back();
                    int direction = getDirectionName_id(i, next);
                    printf("move %d %d\n", i, direction);
                }
            }
            else if (robot[i].path.empty() && robot[i].goal_good != nullptr && robot[i].goal_berth_id == -1) // 有货物目标，前往货物的路已经走完
            {
                if(robot[i].x==robot[i].goal_good->x && robot[i].y==robot[i].goal_good->y){
                    printf("get %d\n", i);                     // 取货
                    robot[i].goal_good->is_disappeared = true; // 货物被拿走，消失了
                }
                else//发生意外,归还货物控制权，重置状态
                {
                    goods.push_back(robot[i].goal_good);
                    robot[i].goal_good = nullptr;
                }
            }  
        }
        else{
            if (robot[i].path.empty() && robot[i].goal_berth_id == -1 && robot[i].goal_good != nullptr) // 无目的地
            {
                pair<int, int> start = {robot[i].x, robot[i].y};
                int berth_id = findNearestBerth(i);
                pair<int, int> end = {berth[berth_id].x, berth[berth_id].y};
                robot[i].goal_berth_id = berth_id;
                threads.emplace_back(searchThread, start, end, ref(mtx), i);
            }
            else if (!robot[i].path.empty() && robot[i].goal_berth_id != -1 && robot[i].goal_good != nullptr) // 有目标，在路上
            {                
                auto next =robot[i].path.back();
                robot[i].path.pop_back();
                int direction = getDirectionName_id(i, next);
                printf("move %d %d\n", i, direction); // 前往目标泊位
            }
            else if (robot[i].path.empty() && robot[i].goal_good != nullptr && robot[i].goal_berth_id != -1) // 到达目的地
            {
                if(robot[i].x==berth[robot[i].goal_berth_id].x && robot[i].y==berth[robot[i].goal_berth_id].y){
                    printf("pull %d\n", i); // 卸货
                    berth[robot[i].goal_berth_id].berth_goods.push_back(robot[i].goal_good->val);
                    sum_value+=robot[i].goal_good->val;
                    sum_num += 1;
                    logError(to_string(id) + "货物总数量:" + to_string(sum_num)+ "总价值:" + to_string(sum_value));
                    delete robot[i].goal_good; // 至此，该货物可以删除了
                    robot[i].goal_good = nullptr; // 重置目标
                    robot[i].goal_berth_id = -1;
                }
                else // 发生意外,重置状态，重新规划路径
                {
                    robot[i].goal_berth_id = -1;
                }
            }
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (boat[i].status == 0 && boat[i].from!= -1 && boat[i].pos !=-1){
            if (15000 - id - berth[boat[i].from].transport_time < 100)
            {
                printf("go %d\n", i);
                logError(to_string(id) + ":boat " + to_string(i) + "go");
            }
            continue;
        }
        else if (boat[i].status == 0 && boat[i].from== -1)
        {
            continue;
        }
        else if(boat[i].status == 2){
            continue;
        }
        else if (boat[i].status == 1 && boat [i].pos == -1)
        {
            boat[i].num = 0;
            int max = -1;
            int index=-1;
            for (int j = 0;j<berth_num;j++)
            {
                bool used=false;
                for (const auto &b : boat)
                {
                    if (b.pos == j){
                        used = true;
                        break;
                    }
                }
                if (used)continue;
                int sum = std::accumulate(berth[j].berth_goods.begin(), berth[j].berth_goods.end(), 0);
                // int sum = berth[j].berth_goods.size();
                if (sum > max)
                {
                    max = sum;
                    index = j;
                }
            }            
            if(index!=-1){
                printf("ship %d %d\n", i, index);
                boat[i].from = -1;
                boat[i].pos = index;
                logError(to_string(id) + ":boat" + to_string(i) + " ship to " + to_string(index));
            }
        }
        else if (boat[i].status == 1 && boat[i].pos != -1)
        {
            if (berth[boat[i].pos].berth_goods.size() < berth[boat[i].pos].loading_speed)
            {
                boat[i].num += berth[boat[i].pos].berth_goods.size();
                berth[boat[i].pos].berth_goods.clear();
            }
            else{
                berth[boat[i].pos].berth_goods.erase(berth[boat[i].pos].berth_goods.begin(), berth[boat[i].pos].berth_goods.begin() + berth[boat[i].pos].loading_speed);
                boat[i].num += berth[boat[i].pos].loading_speed;
            }

            if (15000 - id - berth[boat[i].pos].transport_time < 100)
            {
                printf("go %d\n", i);
                boat[i].from = boat[i].pos;
                boat[i].pos = -1;
                logError(to_string(id) + ":boat " + to_string(i) + "go");
            }
            else if (berth[boat[i].pos].berth_goods.empty() && boat[i].num >boat_capacity-10)
            {
                printf("go %d\n", i);
                boat[i].from = boat[i].pos;
                boat[i].pos = -1;
                logError(to_string(id) + ":boat " + to_string(i) + "go");
            }
            else if (berth[boat[i].pos].berth_goods.empty() && boat[i].num < boat_capacity-10)
            {
                int max = 0;
                int index = -1;
                for (int j = 0; j < berth_num; j++)
                {
                    bool used = false;
                    for (const auto &b : boat)
                    {
                        if (b.pos == j){
                            used = true;
                            break;
                        }
                    }
                    if (used)
                        continue;
                    int sum = std::accumulate(berth[j].berth_goods.begin(), berth[j].berth_goods.end(), 0);
                    // int sum = berth[j].berth_goods.size();
                    if (sum > max)
                    {
                        max = sum;
                        index = j;
                    }
                }
                if(index!=-1){
                    printf("ship %d %d\n", i, index);
                    boat[i].from = boat[i].pos;
                    boat[i].pos = index;
                    logError(to_string(id) + ":boat" + to_string(i) + " ship to " + to_string(index));
                }
            }
        }
    }
}
int main()
{
    Init();
    for (int zhen = 1; zhen <= 15000; zhen++)
    {
        id = Input();
        //  预处理，更新货物数据
        for (auto it = goods.begin(); it != goods.end();)
        {
            auto good = *it;
            if ((!good->is_disappeared) && id > good->die_id)
            {
                it = goods.erase(it);
                delete good;
            }
            else
            ++it; // 移动迭代器
        }
        makeDestination();
        puts("OK");
        fflush(stdout);
    }
    return 0;
}
