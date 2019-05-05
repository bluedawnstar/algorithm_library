#pragma once

// It can be used for initial centroids for k-means.
template <typename T>
struct SimpleClustering {
    struct RandFloat {
        static double get(double min, double max) {
            static mt19937 eng(7);
            //static auto seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
            //static std::default_random_engine eng(seed);
            static uniform_real_distribution<double> dist(min, max);
            return dist(eng);
        }
    };

    static void div(pair<int, int>& p, int size) {
        p.first = (p.first + (size >> 1)) / size;
        p.second = (p.second = (size >> 1)) / size;
    }

    static void div(pair<long long, long long>& p, int size) {
        p.first = (p.first + (size >> 1)) / size;
        p.second = (p.second = (size >> 1)) / size;
    }

    static void div(pair<double, double>& p, int size) {
        p.first = p.first / size;
        p.second = p.second / size;
    }

    static void split0(const vector<pair<T, T>>& points, const vector<int>& group,
        const pair<T, T>& center, vector<int>& g1, vector<int>& g2, bool splitY) {
        g1.clear();
        g2.clear();
        if (splitY) {
            for (auto i : group) {
                if (points[i].second <= center.second)
                    g1.push_back(i);
                else
                    g2.push_back(i);
            }
        } else {
            for (auto i : group) {
                if (points[i].first <= center.first)
                    g1.push_back(i);
                else
                    g2.push_back(i);
            }
        }
    }

#define ALPHA       0.3986          //TODO: tuning point
#define ALPHA_INV   (1.0 / ALPHA)   //TODO: tuning point

    static void split1(const vector<pair<T, T>>& points, const vector<int>& group,
        const pair<T, T>& center, vector<int>& g1, vector<int>& g2, bool splitY) {
        g1.clear();
        g2.clear();

        //double d = RandFloat::get(0.0, 4.0) - 2.0;
        //double M = ALPHA + RandFloat::get(0.0, 1.0) - 0.5; 
        double d = 0.0;     //TODO: tuning point
        double M = ALPHA;   //TODO: tuning point

        if (splitY) {
            for (auto i : group) {
                if (points[i].second <= (points[i].first - center.first) * M + center.second + d)
                    g1.push_back(i);
                else
                    g2.push_back(i);
            }
        } else {
            M = 1.0 / M;
            for (auto i : group) {
                if (points[i].second <= (center.first - points[i].first) * M + center.first + d)
                    g1.push_back(i);
                else
                    g2.push_back(i);
            }
        }
    }

    //---

    //double calcCost(double distSum, int groupCount);
    static double doClustering(bool splitY, const vector<pair<T,T>>& points, int maxGroupSize, int maxGroupCount,
        vector<pair<T,T>>& centers, vector<vector<int>>& groups,
        const function<double(double,int)>& calcCost) {
        int N = int(points.size());

        //--- step #1
        priority_queue<tuple<int, bool, shared_ptr<vector<int>>>> Q;
        Q.emplace(N, splitY, make_shared<vector<int>>(N));
        iota(get<2>(Q.top())->begin(), get<2>(Q.top())->end(), 0);

        while (get<0>(Q.top()) > maxGroupSize) {
            bool splitY = get<1>(Q.top());
            auto c = calcCentroid(points, *get<2>(Q.top()));

            auto g1 = make_shared<vector<int>>();
            auto g2 = make_shared<vector<int>>();
            split1(points, *get<2>(Q.top()), c, *g1, *g2, splitY);
            Q.pop();

            if (!g1->empty())
                Q.emplace(int(g1->size()), !splitY, g1);
            if (!g2->empty())
                Q.emplace(int(g2->size()), !splitY, g2);
        }

        //--- step #2
        priority_queue<tuple<double, bool, shared_ptr<vector<int>>>> Q2;
        double distSum = 0;
        while (!Q.empty()) {
            auto c = calcCentroid(points, *get<2>(Q.top()));
            double d = sumDistance(points, *get<2>(Q.top()), c);
            Q2.emplace(d, get<1>(Q.top()), get<2>(Q.top()));
            Q.pop();

            distSum += d;
        }

        double minCost = calcCost(distSum, int(Q2.size()));
        while ((int)Q2.size() < maxGroupCount && get<0>(Q2.top()) > 0) {
            bool splitY = get<1>(Q2.top());
            auto c = calcCentroid(points, *get<2>(Q2.top()));

            auto g1 = make_shared<vector<int>>();
            auto g2 = make_shared<vector<int>>();
            split1(points, *get<2>(Q2.top()), c, *g1, *g2, splitY);

            double newDistSum = distSum - get<0>(Q2.top());
            int newCnt = int(Q2.size()) - 1;

            double g1D = 0.0;
            if (!g1->empty()) {
                auto g1C = calcCentroid(points, *g1);
                g1D = sumDistance(points, *g1, g1C);
                newDistSum += g1D;
                newCnt++;
            }

            double g2D = 0.0;
            if (!g2->empty()) {
                auto g2C = calcCentroid(points, *g2);
                g2D = sumDistance(points, *g2, g2C);
                newDistSum += g2D;
                newCnt++;
            }

            double cost = calcCost(newDistSum, newCnt);
            if (cost > minCost) {
                double d = get<0>(Q2.top());
                auto g = get<2>(Q2.top());
                Q2.pop();
                Q2.emplace(-d, splitY, g);
                continue;
            }

            distSum = newDistSum;
            minCost = cost;

            Q2.pop();
            if (!g1->empty())
                Q2.emplace(g1D, !splitY, g1);
            if (!g2->empty())
                Q2.emplace(g2D, !splitY, g2);
        }

        //---

        int M = int(Q2.size());
        centers.resize(M);
        groups.reserve(M);
        while (!Q2.empty()) {
            groups.push_back(*get<2>(Q2.top()));
            Q2.pop();
        }

        for (int i = 0; i < M; i++)
            centers[i] = calcCentroid(points, groups[i]);

        return minCost;
    }

//private:
    static pair<T, T> calcCentroid(const vector<pair<T, T>>& points, const vector<int>& group) {
        if (group.size() == 1)
            return points[group[0]];

        int N = int(group.size());
        pair<T, T> res;
        for (auto i : group) {
            res.first += points[i].first;
            res.second += points[i].second;
        }

        div(res, N);

        return res;
    }

    static double calcDist(const pair<T, T>& p1, const pair<T, T>& p2) {
        return sqrt(T(p1.first - p2.first) * T(p1.first - p2.first)
            + T(p1.second - p2.second) * T(p1.second - p2.second));
    }

    static double sumDistance(const vector<pair<T,T>>& points, const vector<int>& group, const pair<T, T>& center) {
        double res = 0.0;
        for (auto i : group)
            res += calcDist(points[i], center);
        return res;
    }

};
