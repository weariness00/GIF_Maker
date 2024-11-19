#pragma once
#include <any>
#include <vector>
#include <functional>
#include <iostream>
class TDelegate
{
public:
    // ��������Ʈ ���� �޼���
    template <typename Res, typename... Args>
    void AddEvent(std::function<Res(Args...)> func) {
        events.emplace_back(func);
    }

    // ��������Ʈ ���� �޼���
    template <typename Res, typename... Args>
    void Execute(Args&&... args)
    {
        for (auto& e : events)
        {
            // �ñ״�ó�� ���� ��쿡�� ����
            if (e.type() == typeid(std::function<Res(Args...)>)) {
                auto extracted_func = std::any_cast<std::function<Res(Args...)>>(e);
                extracted_func(args...);  // ��������Ʈ ����
            }
            else {
                std::cout << "Error: Arguments do not match the expected signature." << std::endl;
            }
        }
    }

    template <typename Res>
    void Execute() const
    {
        for (auto& e : events)
        {
            // �ñ״�ó�� ���� ��쿡�� ����
            if (e.type() == typeid(std::function<Res()>)) {
                auto extracted_func = std::any_cast<std::function<Res()>>(e);
                extracted_func();  // ��������Ʈ ����
            }
            else {
                std::cout << "Error: Arguments do not match the expected signature." << std::endl;
            }
        }
    }

    // ��������Ʈ ���� �޼���
    template <typename Res, typename... Args>
    void RemoveEvent(std::function<Res(Args...)> func) {
        auto it = std::remove_if(events.begin(), events.end(),
            [&](const std::function<Res(Args...)>& storedAction) {
                return storedAction.target_type() == func.target_type();
            });
        events.erase(it, events.end());  // ��ġ�ϴ� ��������Ʈ ����
    }

private:
    std::vector<std::any> events;
};

