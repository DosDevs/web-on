#ifndef WEBON__INI_FILE_H__INCLUDED
#define WEBON__INI_FILE_H__INCLUDED

#include <fstream>
#include <map>

#include "utility.h"

namespace webon
{
  using std::map;
  using std::string;
  using std::string_view;

  class Ini_File
  {
    public:
      using Section = map<string, string>;
      using Section_Map = map<string, Section>;

    private:
      Section_Map _rep;

      template<typename Key, typename Value>
      static Value* _get_from_map_if_exists(map<Key, Value>& which_map, Key const& which_key)
      {
        auto iterator = which_map.find(which_key);
        return (iterator == which_map.end())? nullptr: &iterator->second;
      }

      template<typename Key, typename Value>
      static Value const* _get_from_map_if_exists(map<Key, Value> const& which_map, Key const& which_key)
      {
        auto iterator = which_map.find(which_key);
        return (iterator == which_map.end())? nullptr: &iterator->second;
      }

    public:
      Ini_File():
        _rep{{string{}, {}}}
      {}

      Section* Get_Section(string const& name)
      {
        return _get_from_map_if_exists(_rep, name);
      }

      Section const* Get_Section(string const& name) const
      {
        return _get_from_map_if_exists(_rep, name);
      }

      bool Has_Section(string const& name) const
      {
        return (Get_Section(name) != nullptr);
      }

      bool Add_Section(string const& name)
      {
        if (Has_Section(name))
          return false;

        _rep[name];
        return true;
      }

      string const* Get_Value(string const& section_name, string const& name) const
      {
        Section const* section = Get_Section(section_name);

        if (section == nullptr)
          return nullptr;

        return _get_from_map_if_exists(*section, name);
      }

      bool Has_Value(string const& section_name, string const& name) const
      {
        return (Get_Value(section_name, name) != nullptr);
      }

      bool Add_Value(string const& section_name, string const& name, string const& value)
      {
        Section* section = Get_Section(section_name);

        if ((section == nullptr) || (_get_from_map_if_exists(*section, name) != nullptr))
          return false;

        section->emplace(name, value);
        return true;
      }

      string const* Get_Value(string const& name) const
      {
        return Get_Value(string{}, name);
      }

      bool Has_Value(string const& name) const
      {
        return Has_Value(string{}, name);
      }

      bool Add_Value(string const& name, string const& value)
      {
        return Add_Value(string{}, name, value);
      }

      void Clear()
      {
        _rep.clear();
        _rep.emplace(string{}, Section{});
      }

      bool Is_Empty() const
      {
        return ((_rep.size() == 1) && (_rep.begin()->second.empty()));
      }
  };

  inline std::istream& operator>>(std::istream& stream, Ini_File& ini)
  {
    string line;
    string section_name;

    bool bad = false;

    ini.Clear();

    while (std::getline(stream, line))
    {
      string_view trimmed_line = trim(line);

      // Empty line.
      if (trimmed_line.empty())
        continue;

      // Comment.
      if (trimmed_line[0] == ';')
        continue;

      // New section.
      if ((trimmed_line.length() > 2) && (trimmed_line.front() == '[') && (trimmed_line.back() == ']'))
      {
        section_name = string{trimmed_line.substr(1, trimmed_line.length() - 2)};

        if (!ini.Add_Section(section_name))
        {
          bad = true;
          break;
        }
      }

      // Value.
      size_t pos = trimmed_line.find('=');

      if (pos == string::npos)
      {
        bad = true;
        break;
      }

      string name = string{trimmed_line.substr(0, pos)};
      string value = string{trimmed_line.substr(pos + 1)};

      if (name.empty())
      {
        bad = true;
        break;
      }

      if (!ini.Add_Value(section_name, name, value))
      {
        bad = true;
        break;
      }
    }

    if (bad)
      ini.Clear();

    return stream;
  }
}  // namespace webon

#endif  // WEBON__INI_FILE_H__INCLUDED

