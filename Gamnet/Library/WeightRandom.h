#ifndef GAMNET_WEIGHT_RANDOM_H
#define GAMNET_WEIGHT_RANDOM_H

#include "./Random.h"
#include <ranges>
#include <map>
#include <list>

namespace Gamnet {

    template <class T>
    class WeightRandom
    {
    private:
        struct Element      // 랜덤 엘리먼트
        {
            T   value;      // 랜덤 히트 시 결과물

            int no;         // 통계 추출을 위한 구분자
            int weight;     // 랜덤 가중치
            int min;        // 구간 최소 가중치 값
            int max;        // 구간 최대 가중치 값
        };

        typedef std::vector<Element*> Elements;

        struct Node         // 가중치 트리를 위한 노드
        {
            Node()
                : left(nullptr)
                , right(nullptr)
                , elmt(nullptr)
            {
            }

            ~Node()
            {
                if (nullptr != left)
                {
                    delete left;
                }

                if (nullptr != right)
                {
                    delete right;
                }
            }

            Node* left;
            Node* right;
            Element* elmt;
        };

        int         total_weight;
        Node*       root;
        Elements    elements;
    public:
        WeightRandom()
            : total_weight(0)
            , root(nullptr)
        {
        }

        ~WeightRandom()
        {
            if (nullptr != root)
            {
                delete root;
            }

            for (Element* elmt : elements)
            {
                delete elmt;
            }
        }

        void AddElement(int weight, const T& value)
        {
            static std::atomic<int> unique_no = 0;

            if (0 == weight)
            {
                return;
            }

            Element* elmt = new Element();

            elmt->value = value;

            elmt->no = ++unique_no;
            elmt->weight = weight;
            elmt->min = 0;
            elmt->max = 0;

            elements.push_back(elmt);

            if (nullptr != root)
            {
                delete root;
                root = nullptr;
            }
        }

        const T& Random()
        {
            int weight = Random::Range(1, total_weight);
            Element* elmt = Search(weight);
            return elmt->value;
        }

        int TotalWeight() const
        {
            return total_weight;
        }

        class Statistics
        {
        public:
            struct Row
            {
                Row(const T& value, int weight)
                    : value(value)
                    , weight(weight)
                    , count(0)
                {
                }

                const T& value;
                const int weight;
                int count;
            };

            int loop;
            std::list<std::shared_ptr<Row>> result;
        };

        static Statistics Simulate(WeightRandom<T>& random, int loop = 1000000)
        {
            Statistics statistics;
            statistics.loop = loop;

            std::map<int, std::shared_ptr<Statistics::Row>> rows;

            for (const Element* elmt : random.elements)
            {
                std::shared_ptr<Statistics::Row> row = std::make_shared<Statistics::Row>(elmt->value, elmt->weight);
                rows.insert(std::make_pair(elmt->no, row));
                statistics.result.push_back(row);
            }

            for (int i = 0; i < loop; i++)
            {
                int weight = Random::Range(1, random.total_weight);

                const Element* elmt = random.Search(weight);

                auto itr = rows.find(elmt->no);
                itr->second->count++;
            }

            return statistics;
        }

    private:
        void BuildTree()
        {
            // 가장 hit 확률이 높은 엘리먼트를 트리의 루트로 보내기 위한 정렬
            auto sorted = elements;
            std::sort(sorted.begin(), sorted.end(), [](Element* lhs, Element* rhs)
                {
                    return lhs->weight > rhs->weight;
                }
            );

            int delta = 0;
            for (Element* elmt : sorted)
            {
                int index = (int)sorted.size() / 2 + delta;
                elements[index] = elmt;

                if (0 == delta)
                {
                    delta -= 1;
                    continue;
                }

                if (0 > delta)
                {
                    delta *= -1;
                    continue;
                }

                if (0 < delta)
                {
                    delta *= -1;
                    delta -= 1;
                    continue;
                }
            }

            for (Element* elmt : elements)
            {
                this->total_weight += elmt->weight;
                elmt->min = this->total_weight - elmt->weight + 1;
                elmt->max = this->total_weight;
            }

            root = new Node();
            Build(root, elements);
        }

        void Build(Node* parent, const Elements& elements)
        {
            if (1 == elements.size())
            {
                parent->elmt = elements[0];
                return;
            }

            int middle = (int)elements.size() / 2;

            parent->elmt = elements[middle];

            auto left = elements | std::ranges::views::take(middle);
            if (0 < left.size())
            {
                parent->left = new Node();
                Build(parent->left, Elements(left.begin(), left.end()));
            }

            auto right = elements | std::ranges::views::drop(middle + 1);
            if (0 < right.size())
            {
                parent->right = new Node();
                Build(parent->right, Elements(right.begin(), right.end()));
            }
        }

        const Element* Search(int weight)
        {
            if (nullptr == root)
            {
                BuildTree();
            }

            const Node* curr = root;
            
            while (nullptr != curr)
            {
                if (curr->elmt->min <= weight && weight <= curr->elmt->max)
                {
                    return curr->elmt;
                }

                if (curr->elmt->min > weight)
                {
                    curr = curr->left;
                }
                else
                {
                    curr = curr->right;
                }
            }

            return nullptr;
        }
    };
}
#endif
