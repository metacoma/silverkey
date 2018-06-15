#ifndef ROBOTHELPER_H
#define ROBOTHELPER_H

#include <string>
#include <map>

using namespace std;

class RobotHelper
{
public:
    RobotHelper();
    string convert(string value);
private:
    map<char, string> keymap;
};

#endif // ROBOTHELPER_H
