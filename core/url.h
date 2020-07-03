#ifndef WEBON__ADDRESS__URL_H__INCLUDED
#define WEBON__ADDRESS__URL_H__INCLUDED

#include <map>
#include <string>
#include <vector>

namespace webon
{
  using std::map;
  using std::string;

  namespace address
  {
    struct URI
    {
      virtual string Get_Namespace() const = 0;
      virtual string Get_Name() const = 0;

      virtual ~URI() {}
    };  // struct URI

    class URL: public URI
    {
      private:
        string _namespace;
        string _host;
        string _port;
        string _resource;
        map<string, string> _kvps;

        static bool _find(string const& what, string const& where, size_t& start, size_t& end, string& output);
        void _extract_namespace(string source, size_t& start, size_t& end);
        bool _extract_host_and_port(string source, size_t& start, size_t& end);
        void _extract_resource(string source, size_t& start, size_t& end);
        bool _extract_kvps(string source, size_t& start, size_t& end);

      public:
        static URL Parse(string const& source);

        void Clear()
        {
          _namespace.clear();
          _host.clear();
          _resource.clear();
          _kvps.clear();
        }

        operator bool() const { return !_namespace.empty(); }
        string Get_Namespace() const override { return _namespace; }
        string Get_Name() const override { return _namespace; }
    };
  }  // namespace address
}  // namespace webon
#endif  // WEBON__ADDRESS__URL_H__INCLUDED

