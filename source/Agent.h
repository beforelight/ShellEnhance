#ifndef MARKMAKER_AGENT_H
#define MARKMAKER_AGENT_H
#include <memory>
#include <string>
#include <utility>
#include <vector>
namespace Agent {
    class Agent {
    public:
        class Object {
        private:
            std::string m_id;
            std::string m_type;

        public:
            const std::string &Id() { return m_id; }
            const std::string &Type() { return m_type; }
            Object(std::string id, std::string type) : m_id(std::move(id)), m_type(std::move(type)) {}
            virtual ~Object() = default;
        };

        // 代理一个已知类型的智能指针
        template<class Tp, const char *typeStr>
        class Share_Ptr : public Object {
        public:
            constexpr static const char *TypeStr = typeStr;
            Share_Ptr(std::shared_ptr<Tp> ptr, std::string id) : m_ptr(std::move(ptr)), Object(std::move(id), TypeStr) {}
            std::shared_ptr<Tp> &get() { return m_ptr; }
        private:
            std::shared_ptr<Tp> m_ptr;
        };

        // 代理一个已知指针
        template<class Tp>
        class Ptr : public Object {
        public:
            Ptr(const char *TypeStr, Tp *ptr, std::string id) : m_ptr(std::move(ptr)), Object(std::move(id), TypeStr) {}
            Tp *&get() { return m_ptr; }
        private:
            Tp *m_ptr;
        };

        std::shared_ptr<Object> find(const std::string &id, const std::string &type);
        std::shared_ptr<Object> append(std::shared_ptr<Object> obj);
        std::vector<std::shared_ptr<Object> > findById(const std::string &id);
        std::vector<std::shared_ptr<Object> > findByType(const std::string &type);
        static Agent &instance();

    private:
        Agent() = default;
        Agent(const Agent &r) = default;
        Agent(Agent &&r) = default;
        Agent &operator=(const Agent &r) = default;
        Agent &operator=(Agent &&r) = default;
        std::vector<std::shared_ptr<Object> > m_objs;
    };
} // namespace Agent
#endif // MARKMAKER_AGENT_H
