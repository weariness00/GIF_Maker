#pragma once
#include <any>
#include <vector>
#include <functional>
#include <iostream>
class TDelegate
{
public:
    // 델리게이트 설정 메서드
    template <typename Res, typename... Args>
    void AddEvent(std::function<Res(Args...)> func) {
        events.emplace_back(func);
    }

    // 델리게이트 실행 메서드
    template <typename Res, typename... Args>
    void Execute(Args&&... args)
    {
        for (auto& e : events)
        {
            // 시그니처가 맞을 경우에만 실행
            if (e.type() == typeid(std::function<Res(Args...)>)) {
                auto extracted_func = std::any_cast<std::function<Res(Args...)>>(e);
                extracted_func(args...);  // 델리게이트 실행
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
            // 시그니처가 맞을 경우에만 실행
            if (e.type() == typeid(std::function<Res()>)) {
                auto extracted_func = std::any_cast<std::function<Res()>>(e);
                extracted_func();  // 델리게이트 실행
            }
            else {
                std::cout << "Error: Arguments do not match the expected signature." << std::endl;
            }
        }
    }

    // 델리게이트 제거 메서드
    template <typename Res, typename... Args>
    void RemoveEvent(std::function<Res(Args...)> func) {
        auto it = std::remove_if(events.begin(), events.end(),
            [&](const std::function<Res(Args...)>& storedAction) {
                return storedAction.target_type() == func.target_type();
            });
        events.erase(it, events.end());  // 일치하는 델리게이트 제거
    }

private:
    std::vector<std::any> events;
};

