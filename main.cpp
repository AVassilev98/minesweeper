#include <assert.h>
#include <iostream>
#include <random>
#include <stack>
#include <vector>

const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string black("\033[0;30m");
const std::string white("\033[0;97m");
const std::string darkGrey("\033[0;90m");
const std::string lightGreen("\033[0;92m");
const std::string lightRed("\033[0;91m");
const std::string lightMagenta("\033[0;95m");
const std::string blue("\033[0;34m");
const std::string reset("\033[0m");

const uint8_t smallBoardDim = 25;
const uint8_t normalBoardDim = 50;
const uint8_t largeBoardDim = 100;

const float easyMinePercent = 0.1f;
const float medMinePercent = 0.25f;
const float hardMinePercent = 0.5f;

enum class Difficulty
{
  EASY,
  MEDIUM,
  HARD
};

enum class BoardSize
{
  SMALL,
  NORMAL,
  LARGE
};

enum class Command
{
  FLAG,
  UNCOVER
};

enum class BoardElement
{
  SAFE_FLAG,
  MINE_FLAG,
  MINE_PRESSED,
  MINE_UNPRESSED,
  SAFE_UNPRESSED,
  ZERO,
  ONE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
};

struct Cell
{
  uint8_t x;
  uint8_t y;
};

class Game
{

private:
  bool m_lost = false;
  std::vector<BoardElement> m_elems;
  uint8_t m_stride = 0;

  void SpreadAtIdx(const uint8_t idxX, const uint8_t idxY)
  {
    std::stack<Cell> recurse_stack;
    recurse_stack.push({idxX, idxY});

    while (!recurse_stack.empty())
    {
      Cell curCell = recurse_stack.top();
      recurse_stack.pop();

      uint16_t currentLoc = curCell.x + curCell.y * m_stride;

      if (m_elems[currentLoc] != BoardElement::SAFE_UNPRESSED && m_elems[currentLoc] != BoardElement::SAFE_FLAG)
        continue;

      BoardElement numSurrounding = getNumSurrounding(curCell);
      m_elems[currentLoc] = numSurrounding;
      if (numSurrounding == BoardElement::ZERO)
      {
        bool lookLeft = curCell.x != 0;
        bool lookRight = curCell.x != m_stride - 1;
        bool lookUp = curCell.y != 0;
        bool lookDown = curCell.x != m_stride - 1;

        if (lookLeft)
          recurse_stack.push({static_cast<uint8_t>(curCell.x - 1), curCell.y});
        if (lookRight)
          recurse_stack.push({static_cast<uint8_t>(curCell.x + 1), curCell.y});
        if (lookUp)
          recurse_stack.push({curCell.x, static_cast<uint8_t>(curCell.y - 1)});
        if (lookDown)
          recurse_stack.push({curCell.x, static_cast<uint8_t>(curCell.y + 1)});
        if (lookLeft && lookUp)
          recurse_stack.push({static_cast<uint8_t>(curCell.x - 1), static_cast<uint8_t>(curCell.y - 1)});
        if (lookLeft && lookDown)
          recurse_stack.push({static_cast<uint8_t>(curCell.x - 1), static_cast<uint8_t>(curCell.y + 1)});
        if (lookRight && lookUp)
          recurse_stack.push({static_cast<uint8_t>(curCell.x + 1), static_cast<uint8_t>(curCell.y - 1)});
        if (lookRight && lookDown)
          recurse_stack.push({static_cast<uint8_t>(curCell.x + 1), static_cast<uint8_t>(curCell.y + 1)});
      }
    }
  }

  BoardElement getNumSurrounding(const Cell &c)
  {
    bool lookLeft = c.x != 0;
    bool lookRight = c.x != m_stride - 1;
    bool lookUp = c.y != 0;
    bool lookDown = c.x != m_stride - 1;

    uint8_t numSurrounding = 0;
    uint16_t currentLoc = c.x + c.y * m_stride;

    if (lookLeft)
    {
      if (m_elems[currentLoc - 1] == BoardElement::MINE_FLAG)
        numSurrounding++;
      else if (m_elems[currentLoc - 1] == BoardElement::MINE_UNPRESSED)
        numSurrounding++;
    }
    if (lookRight)
    {
      if (m_elems[currentLoc + 1] == BoardElement::MINE_FLAG)
        numSurrounding++;
      else if (m_elems[currentLoc + 1] == BoardElement::MINE_UNPRESSED)
        numSurrounding++;
    }
    if (lookUp)
    {
      if (m_elems[currentLoc - m_stride] == BoardElement::MINE_FLAG)
        numSurrounding++;
      else if (m_elems[currentLoc - m_stride] == BoardElement::MINE_UNPRESSED)
        numSurrounding++;
    }
    if (lookDown)
    {
      if (m_elems[currentLoc + m_stride] == BoardElement::MINE_FLAG)
        numSurrounding++;
      else if (m_elems[currentLoc + m_stride] == BoardElement::MINE_UNPRESSED)
        numSurrounding++;
    }
    if (lookLeft && lookUp)
    {
      if (m_elems[currentLoc - m_stride - 1] == BoardElement::MINE_FLAG)
        numSurrounding++;
      else if (m_elems[currentLoc - m_stride - 1] == BoardElement::MINE_UNPRESSED)
        numSurrounding++;
    }
    if (lookLeft && lookDown)
    {
      if (m_elems[currentLoc + m_stride - 1] == BoardElement::MINE_FLAG)
        numSurrounding++;
      else if (m_elems[currentLoc + m_stride - 1] == BoardElement::MINE_UNPRESSED)
        numSurrounding++;
    }
    if (lookRight && lookUp)
    {
      if (m_elems[currentLoc - m_stride + 1] == BoardElement::MINE_FLAG)
        numSurrounding++;
      else if (m_elems[currentLoc - m_stride + 1] == BoardElement::MINE_UNPRESSED)
        numSurrounding++;
    }
    if (lookRight && lookDown)
    {
      if (m_elems[currentLoc + m_stride + 1] == BoardElement::MINE_FLAG)
        numSurrounding++;
      else if (m_elems[currentLoc + m_stride + 1] == BoardElement::MINE_UNPRESSED)
        numSurrounding++;
    }

    switch (numSurrounding)
    {
    case 0:
      return BoardElement::ZERO;
    case 1:
      return BoardElement::ONE;
    case 2:
      return BoardElement::TWO;
    case 3:
      return BoardElement::THREE;
    case 4:
      return BoardElement::FOUR;
    case 5:
      return BoardElement::FIVE;
    case 6:
      return BoardElement::SIX;
    case 7:
      return BoardElement::SEVEN;
    case 8:
      return BoardElement::EIGHT;
    default:
      assert(0 && "More than possible surrounding!");
    }
  }

public:
  Game(const Difficulty diff, const BoardSize size)
  {

    unsigned numElems = 0;
    unsigned numMines = 0;

    switch (size)
    {
    case BoardSize::SMALL:
      m_stride = smallBoardDim;
      numElems = smallBoardDim * smallBoardDim;
      break;
    case BoardSize::NORMAL:
      m_stride = normalBoardDim;
      numElems = normalBoardDim * normalBoardDim;
      break;
    case BoardSize::LARGE:
      m_stride = largeBoardDim;
      numElems = largeBoardDim * largeBoardDim;
      break;
    default:
      assert(0 && "Invalid board size value");
      break;
    }

    switch (diff)
    {
    case Difficulty::EASY:
      numMines = numElems * easyMinePercent;
      break;
    case Difficulty::MEDIUM:
      numMines = numElems * medMinePercent;
      break;
    case Difficulty::HARD:
      numMines = numElems * hardMinePercent;
      break;
    default:
      assert(0 && "Invalid difficulty value");
      break;
    }

    m_elems.reserve(numElems);

    for (int i = 0; i < m_elems.capacity(); ++i)
    {

      m_elems.emplace_back<BoardElement>(BoardElement::SAFE_UNPRESSED);
    }

    std::default_random_engine generator;
    std::uniform_int_distribution<uint16_t> distribution(1, numElems - 1);

    for (unsigned i = 0; i < numMines; ++i)
    {
      m_elems[distribution(generator)] = BoardElement::MINE_UNPRESSED;
    }
  }

  void updateBoard(const uint8_t idxX, const uint8_t idxY, const Command cmd)
  {
    switch (cmd)
    {
    case Command::FLAG:
      switch (auto &elem = m_elems[idxY * m_stride + idxX])
      {
      case BoardElement::MINE_UNPRESSED:
        elem = BoardElement::MINE_FLAG;
        break;
      default:
        elem = BoardElement::SAFE_FLAG;
      }
      break;
    case Command::UNCOVER:
      switch (auto &elem = m_elems[idxY * m_stride + idxX])
      {
      case BoardElement::MINE_FLAG:
      case BoardElement::MINE_UNPRESSED:
        elem = BoardElement::MINE_PRESSED;
        m_lost = true;
        break;
      default:
        SpreadAtIdx(idxX, idxY);
        break;
      }
      break;
    }
  }

  void printBoard() const
  {
    uint8_t elem;
    std::string color;

    for (int i = 0; i < m_stride; i++)
    {
      for (int j = 0; j < m_stride; j++)
      {
        switch (m_elems[i * m_stride + j])
        {
        case BoardElement::MINE_PRESSED:
          elem = 'M';
          color = black;
          break;
        case BoardElement::SAFE_FLAG:
        case BoardElement::MINE_FLAG:
          elem = 'F';
          color = yellow;
          break;
        case BoardElement::SAFE_UNPRESSED:
        case BoardElement::MINE_UNPRESSED:
          elem = '/';
          color = white;
          break;
        case BoardElement::ZERO:
          elem = '0';
          color = darkGrey;
          break;
        case BoardElement::ONE:
          elem = '1';
          color = green;
          break;
        case BoardElement::TWO:
          elem = '2';
          color = cyan;
          break;
        case BoardElement::THREE:
          elem = '3';
          color = blue;
          break;
        case BoardElement::FOUR:
          elem = '4';
          color = red;
          break;
        case BoardElement::FIVE:
          elem = '5';
          color = magenta;
          break;
        case BoardElement::SIX:
          elem = '6';
          color = lightRed;
          break;
        case BoardElement::SEVEN:
          elem = '7';
          color = lightGreen;
          break;
        case BoardElement::EIGHT:
          elem = '8';
          color = lightMagenta;
          break;
        default:
          assert(0 && "Invalid element");
        }

        std::cout << color << elem << ' ';
      }
      std::cout << '\n';
    }

    if (m_lost)
      std::cout << "YOU LOSE";
    std::cout << '\r';
  }

  bool lost() const { return m_lost; }
  uint8_t width() const { return m_stride; }
};

int main(int argc, char *argv[])
{

  int diff;
  int size;

  std::cout << "Please enter difficulty (0 - 2)\n";
  std::cin >> diff;
  std::cout << "Please enter board size (0 - 2)\n";
  std::cin >> size;

  Difficulty enum_diff;
  BoardSize enum_size;

  switch (diff)
  {
  case 0:
    enum_diff = Difficulty::EASY;
    break;
  case 1:
    enum_diff = Difficulty::MEDIUM;
    break;
  case 2:
    enum_diff = Difficulty::HARD;
    break;
  default:
    throw std::invalid_argument("Invalid difficulty!\n");
  }

  switch (size)
  {
  case 0:
    enum_size = BoardSize::SMALL;
    break;
  case 1:
    enum_size = BoardSize::NORMAL;
    break;
  case 2:
    enum_size = BoardSize::LARGE;
    break;
  default:
    throw std::invalid_argument("Invalid size!\n");
  }

  Game g(enum_diff, enum_size);
  g.printBoard();

  int idxX = 0;
  int idxY = 0;

  while (!g.lost())
  {
    std::cout << "Please enter X index (0 - " << (int)g.width() - 1 << ")\n";
    std::cin >> idxX;
    std::cout << "Please enter Y index (0 - " << (int)g.width() - 1 << ")\n";
    std::cin >> idxY;

    if (idxX < 0 || idxX > g.width() - 1)
      throw std::invalid_argument("Invalid Location!\n");
    if (idxY < 0 || idxY > g.width() - 1)
      throw std::invalid_argument("Invalid Location!\n");

    g.updateBoard(idxX, idxY, Command::UNCOVER);
    g.printBoard();
  }
}