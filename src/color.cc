#include "stage-simulator/stage.hh"
#include "stage-simulator/worldfile.hh"
using namespace Stg;

#include "stage-simulator/file_manager.hh"

#include <errno.h>
#include <sstream>
#include <embed_resource/loader.h>

Color::Color(double r, double g, double b, double a) : r(r), g(g), b(b), a(a)
{
}

Color::Color() : r(1.0), g(0.0), b(0.0), a(1.0)
{
}

bool Color::operator!=(const Color &other) const
{
  double epsilon = 1e-4; // small
  return (fabs(r - other.r) > epsilon || fabs(g - other.g) > epsilon || fabs(b - other.b) > epsilon
          || fabs(a - other.a) > epsilon);
}

Color::Color(const std::string &name) : r(1), g(0), b(0), a(1)
{
  if (name == "") // empty string?
    return; // red

  static bool loaded = false;
  static std::map<std::string, Color> table;

  if (!loaded) {
    loaded = true;
    Resource res = LOAD_RESOURCE(assets_rgb_txt);
    std::istringstream buffer(res.data());
    std::string line;
    // load the data into the map
    while (getline(buffer, line)) {
      // it's a macro or comment line - ignore the line
      // also ignore empty lines
      if (line[0] == '!' || line[0] == '#' || line[0] == '%' || line[0] == '\0')
        continue;

      // Trim the trailing space
      line.erase(line.find_last_not_of(" \t\n") + 1);

      // Read the color
      int r, g, b;
      int chars_matched = 0;
      sscanf(line.c_str(), "%d %d %d %n", &r, &g, &b, &chars_matched);

      // Read the name
      const char *colorname = line.c_str() + chars_matched;

      // map the name to the color in the table
      table[colorname] = Color(r / 255.0, g / 255.0, b / 255.0);
    }
  }

  // look up the colorname in the database
  Color &found = table[name];

  this->r = found.r;
  this->g = found.g;
  this->b = found.b;
  this->a = found.a;
}

bool Color::operator==(const Color &other) const
{
  return (!((*this) != other));
}

Color Color::RandomColor()
{
  return Color(drand48(), drand48(), drand48());
}

void Color::Print(const char *prefix) const
{
  printf("%s [%.2f %.2f %.2f %.2f]\n", prefix, r, g, b, a);
}

const Color &Color::Load(Worldfile *wf, const int section)
{
  if (wf->PropertyExists(section, "color")) {
    const std::string &colorstr = wf->ReadString(section, "color", "");

    if (colorstr != "") {
      if (colorstr == "random") {
        r = drand48();
        g = drand48();
        b = drand48();
        a = 1.0;
      } else {
        Color c = Color(colorstr);
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
      }
    }
  } else
    wf->ReadTuple(section, "color_rgba", 0, 4, "ffff", &r, &g, &b, &a);

  return *this;
}
