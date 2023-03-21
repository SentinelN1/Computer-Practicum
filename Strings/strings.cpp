// ЗАДАЧА №857
// Abracadabra

#include <fstream>
#include <string>
#include <vector>

int main()
{
    int n, m;
    std::ifstream ifs;
    ifs.open("INPUT.TXT");

    ifs >> n;
    std::vector<std::string> abracadabras(n);
    for (int i = 0; i < n; i++)
    {
        ifs >> abracadabras[i];
    }

    ifs >> m;
    std::ofstream ofs;
    ofs.open("OUTPUT.TXT");
    for (int i = 0; i < m; i++)
    {
        std::string tmp;
        ifs >> tmp;
        int count = 0;
        for (auto str : abracadabras)
        {
            int strlen = str.length();
            int tmplen = tmp.length();
            if (str.find(tmp) == 0 && str.rfind(tmp) == strlen - tmplen)
            {
                ++count;
            }
        }
        ofs << count << "\n";
    }
    ifs.close();
    ofs.close();

    return 0;
}
