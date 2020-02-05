#ifndef TOWER_H
#define TOWER_H

#include <string>
#include <vector>

class towerclass
{
public:
    towerclass();

    int backat(int xp, int yp, int yoff);

    int at(int xp, int yp, int yoff);

    int miniat(int xp, int yp, int yoff);

    void fillbackground(short *tmap);

    void fillminitower(short *tmap);

    void loadminitower1();

    void loadminitower2();

    void loadbackground();

    void fillcontents(short *tmap);

    void loadmap();

    //public var back:Array = new Array();
    //public var contents:Array = new Array();
    //public var minitower:Array = new Array();
    //public var vmult:Array = new Array();

    short back[120*40];
    short contents[700*40];
    short minitower[100*40];
    std::vector<int> vmult;

    bool minitowermode;
    int i;
    int k;
};





#endif /* TOWER_H */
