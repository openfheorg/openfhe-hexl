// take two tsv files representing spreadsheets
// and produce a differential report

#include <fstream>
#include <iostream>
#include <limits>
#include <regex>
#include <set>
#include <string>
#include <vector>

using namespace std;

void abort(const string & msg)
{
  cerr << "abort: " << msg << endl;
  exit(1);
}

bool is_number(const string &token)
{
  return regex_match(token, regex(("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?")));
}

double to_double(const string & s)
{
  stringstream ss;
  ss << s;
  double d;
  ss >> d;
  return d;
}

bool extremes(vector<string> values, set<int> indices, double tolerance, int & smallest, int & largest)
{
  smallest = -1;
  largest  = -1;

  bool valid_index = false;
  double smallest_value = std::numeric_limits<double>::max();
  double largest_value  = std::numeric_limits<double>::min();
  for (auto i : indices)
  {
    string value = values[i];
    if (is_number(value))
    {
      double d = to_double(value);
      if (d < smallest_value)
      {
        smallest = i;
        smallest_value = d;
      }
      else if (d > largest_value)
      {
        largest = i;
        largest_value = d;
      }
    }
  }

  return (smallest != -1) && (largest != -1) && (smallest_value < largest_value * (1.0 - tolerance));
}

bool delta_pct(double d0, double d1, double tolerance)
{
  double ratio = 0.0;
  if (d1 != 0.0)
    ratio = d0 / d1;
  bool rc = (ratio < (1.0 + tolerance) && ratio > (1.0 - tolerance));
  return rc;
}

int main(int argc, char** argv)
{
  if (argc != 2)
    abort("usage: bench-color-html [tolerance]");
  double tolerance = to_double(argv[1]);

  cout << "<!DOCTYPE html><html><head><!-- --></head><body style=\"background-color:#000000;color:#808080\">" << endl;

  // read loop
  char buf[1024];
  bool headers_found = false;
  bool in_table = false;
  set<int> speedup;
  while (!cin.eof())
  {
    if (cin.fail())
      abort("first file failure");

    // read 0
    cin.getline(buf, sizeof(buf));
    if (cin.fail())
      break;

    // splitting regex
    regex reg_tab("\\t+");

    // tokenize 0
    string line(buf);
    sregex_token_iterator iter(line.begin(), line.end(), reg_tab, -1);
    sregex_token_iterator end;
    vector<string> vec(iter, end);

    if (!headers_found && vec.size() > 1)
    {
      cout << "<table>" << endl;
      in_table = true;
    }

    int smallest, largest;
    bool extremes_found = extremes(vec, speedup, tolerance, smallest, largest);
    double ds = std::numeric_limits<double>::min();
    double dl = std::numeric_limits<double>::max();
    if (extremes_found)
    {
      ds = to_double(vec[smallest]);
      dl = to_double(vec[largest]);
    }
    
    cout << "<tr>" << endl;

    // cursor over tokens
    vector<string>::iterator cursor = vec.begin();
    int token_count = 0;
    while (cursor != vec.end())
    {
      bool is_num = is_number(*cursor);

      if (!is_num)
      {
        if (in_table)
          cout << "<td style=\"text-align:center\"><b>" << *cursor << "</b></td>\t";
        else
          cout << "<b>" << *cursor << "</b>";
        if (*cursor == "speedup")
        {
          speedup.insert(token_count);
          headers_found = true;
        }
      }
      else
      {
        double d  = to_double(*cursor);
        bool is_speedup = speedup.find(token_count) != speedup.end();
        if (is_speedup && extremes_found)
        {
          bool smallest_close = delta_pct(ds, d, tolerance / 2.0);
          bool largest_close  = delta_pct(dl, d, tolerance / 2.0);
          if (token_count == largest || largest_close)
          {
            cout << "<td style=\"color:#00ff00;text-align:center\">" << d << "</td>" << endl;
          }
          else if (token_count == smallest || smallest_close)
          {
            cout << "<td style=\"color:#ff0000;text-align:center\">" << d << "</td>" << endl;
          }
          else if (d < 1.0 - tolerance)
          {
            cout << "<td style=\"color:#ffff00;text-align:center\">" << d << "</td>" << endl;
          }
          else if (d > 1.0 + tolerance)
          {
            cout << "<td style=\"color:#0000ff;text-align:center\">" << d << "</td>" << endl;
          }
          else
          {
            cout << "<td style=\"text-align:center\">" << d << "</td>" << endl;
          }
        }
        else
        {
          cout << "<td style=\"text-align:center\">" << d << "</td>" << endl;
        }
      }
    
      // move on
      cursor++;
      token_count++;
    }
    // next line
    if (in_table)
      cout << "</tr>" << endl;
    else
      cout << "<br>" << endl;
  }
  cout << "</body></html>" << endl;
}
