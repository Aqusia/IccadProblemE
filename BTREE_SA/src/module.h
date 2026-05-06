#ifndef MODULE_H
#define MODULE_H

#include <vector>
#include <string>
using namespace std;

/**
 * Terminal stores a named geometry endpoint used by the netlist.
 * Fixed terminals are modeled as points, while movable blocks reuse the same
 * coordinate fields to store their placed bounding boxes.
 */
class Terminal
{
public:
    /** Build a terminal from the coordinates parsed from the input file. */
    Terminal(const string &name, size_t x, size_t y) : _name(name), _x1(x), _y1(y), _x2(x), _y2(y) {}
    ~Terminal() {}

    /** Basic accessors. */
    const string &getName() const { return _name; }
    size_t getX1() const { return _x1; }
    size_t getX2() const { return _x2; }
    size_t getY1() const { return _y1; }
    size_t getY2() const { return _y2; }

    /** Update the terminal name or geometry. */
    void setName(const string &name) { _name = name; }
    void setPos(size_t x1, size_t y1, size_t x2, size_t y2)
    {
        _x1 = x1;
        _y1 = y1;
        _x2 = x2;
        _y2 = y2;
    }

protected:
    string _name; // Object name.
    size_t _x1;   // Lower-left x.
    size_t _y1;   // Lower-left y.
    size_t _x2;   // Upper-right x.
    size_t _y2;   // Upper-right y.
};

/**
 * Block represents a movable hard macro.
 * It inherits Terminal so the placed bounding box lives in the same fields.
 */
class Block : public Terminal
{
public:
    /** Build a block with width and height before placement is known. */
    Block(const string &name, size_t w, size_t h) : Terminal(name, 0, 0), _w(w), _h(h)
    {
        rotate = false;
    }
    ~Block() {}

    /**
     * Width and height helpers.
     * getWidth/getHeight accept a temporary rotation choice, while
     * getCurrentWidth/getCurrentHeight use the current orientation.
     */
    size_t getWidth(bool rotate = false) const { return rotate ? _h : _w; }
    size_t getHeight(bool rotate = false) const { return rotate ? _w : _h; }
    size_t getCurrentWidth() const { return rotate ? _h : _w; }
    size_t getCurrentHeight() const { return rotate ? _w : _h; }
    size_t getArea() const { return _h * _w; }
    static size_t getMaxX() { return _maxX; }
    static size_t getMaxY() { return _maxY; }
    bool isRotated() const { return rotate; }

    /** Rotation swaps the view of width and height without rewriting _w/_h. */
    void setWidth(size_t w) { _w = w; }
    void setHeight(size_t h) { _h = h; }
    static void setMaxX(size_t x) { _maxX = x; }
    static void setMaxY(size_t y) { _maxY = y; }
    void setRotate() { rotate = !(rotate); }
    void setRotate(bool rotated) { rotate = rotated; }

private:
    size_t _w;           // Original width.
    size_t _h;           // Original height.
    static size_t _maxX; // Current outline max x.
    static size_t _maxY; // Current outline max y.
    bool rotate;         // True when the rotated orientation is active.
};

const int NIL = -1;

/**
 * B*-tree node stored with indices instead of raw pointers.
 * blockIdx references the real Block inside _blkList.
 */
struct BStarNode
{
    explicit BStarNode(int idx = NIL) : blockIdx(idx), parent(NIL), left(NIL), right(NIL) {}

    /** Return true when the node has no children. */
    bool isLeaf() const
    {
        return left == NIL && right == NIL;
    }

    int blockIdx; // Referenced block index.
    int parent;   // Parent node index.
    int left;     // Left child index.
    int right;    // Right child index.
};

/** Net stores the terminals and blocks connected by one netlist edge. */
class Net
{
public:
    Net() {}
    ~Net() {}

    const vector<Terminal *> &getTermList() const { return _termList; }

    void addTerm(Terminal *term) { _termList.push_back(term); }

    /** Compute HPWL from the current geometry of all connected pins. */
    double calcHPWL();

private:
    vector<Terminal *> _termList; // All pins or blocks connected by the net.
};

#endif // MODULE_H
