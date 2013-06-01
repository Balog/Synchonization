#ifndef CLASSFACTORY_H
#define CLASSFACTORY_H
#include <map>
using namespace std;

#include <string>
#include <map>

//шаблон фабрики для обработки ошибок в случае, если указатель на функцию не найден
//на него особо можно не обращать пока внимания - просто переписать как есть
//ничего значительного он не делает
template <typename idtype, class product>
struct def_err_policy {
    struct excpt : public std::exception {
        const char* what() const throw() {
            return "fabric called with unknown type";
        }
    };
    static product* on_unknown_type(const idtype& ){throw excpt();}
};

//шаблон фабрики:
template <
        class product, //абстрактный класс для классов-реализаций
        typename idtype, //тип значений для идентификации классов-реализаций
        class creator = product* (*)(), //указатель на функцию, создающую объект класса-реализации
        template <typename,class>
        class factory_err_policy = def_err_policy //тот самый шаблон выше - при желании можно вызывать шаблон со своей реализацией
        >
class tVfactory {
private:
    typedef std::map<idtype,creator> assoc_map; //описание типа для ассоциативного массива
    assoc_map assoc_; //ассоциативный массив
public:
    tVfactory() :assoc_() {} //конструктор с инициализацией пустого ассоциативного массива
    ~tVfactory() { //деструктор, стирающий все данные из ассоциативного массива
        assoc_.erase(assoc_.begin(), assoc_.end());
    }
    product* create(const idtype& id) { //метод, возвращающий по наименованию объекта класса-реализации ссылку на этот объект
        typename assoc_map::const_iterator i = assoc_.find(id);
        if(i != assoc_.end()) { return (i->second)(); }
        return factory_err_policy<idtype,product>::on_unknown_type(id); //тот самый шаблон :) можно упростить до "throw std::runtime_err()"
    }
    bool reg(const idtype& id,creator cr){ //регистрация объекта класса-реализации - или просто добавление в ассоц. массив
        return assoc_.insert(typename assoc_map::value_type(id, cr)).second != 0;
    }
    bool unreg(const idtype& id){ //удаление объекта класса-реализации из ассоц.массива
        return assoc_.erase(id) != 0;
    }
};

//описание абстрактного класса реализации:
#endif // CLASSFACTORY_H
