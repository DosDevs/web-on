#include <string>
#include <vector>
#include <map>

namespace JSON
{
    enum class ValueType
    {
        Object,
        Array,
        String,
        Number,
        Boolean,
        Null
    };

    class Value
    {
    private:
        ValueType _type = ValueType::Null;
        std::string _string_value;
        std::vector<Value> _array_value;
        std::map<std::string, Value> _object_value;
        double _number_value;
        bool _boolean_value;

    public:
        void SetStringValue(std::string value) {
            _type = ValueType::String;
            _string_value = value;
        }
        void SetArrayValue(std::vector<Value> value) {
            _type = ValueType::Array;
            _array_value = value;
        }
        void SetObjectValue(std::map<std::string, Value> value) {
            _type = ValueType::Object;
            _object_value = value;
        }
        void SetNumberValue(double value) {
            _type = ValueType::Number;
            _number_value = value;
        }
        void SetBooleanValue(bool value) {
            _type = ValueType::Boolean;
            _boolean_value = value;
        }
        void SetNullValue() {
            _type = ValueType::Boolean;
        }
        ValueType GetType() const
        {
            return _type;
        };
        bool GetStringValue(std::string &value) const
        {
            if (_type == ValueType::String)
            {
                value = _string_value;
                return true;
            }
            return false;
        };
        bool GetArrayValue(std::vector<Value> &value) const
        {
            if (_type == ValueType::Array)
            {
                value = _array_value;
                return true;
            }
            return false;
        };
        bool GetNumberValue(double &value) const
        {
            if (_type == ValueType::Number)
            {
                value = _number_value;
                return true;
            }
            return false;
        };
        bool GetBooleanValue(bool &value) const
        {
            if (_type == ValueType::Boolean)
            {
                value = _boolean_value;
                return true;
            }
            return false;
        };
        bool GetObjectValue(std::map<std::string, Value> &value) const
        {
            if (_type == ValueType::Object)
            {
                value = _object_value;
                return true;
            }
            return false;
        };
    };

    enum TokenType { Symbol, String, Boolean, Number, Null};
    struct Token {
        TokenType type;
        std::string value;
    };

    class Parser
    {        
    public:
        static std::vector<Token> lex(const std::string &string)
        {
            std::vector<Token> tokens;
            std::string buffer;
            Token token;
            for (std::string::size_type i = 0; i < string.size(); i++)
            {
                buffer = "";
                char c = string[i];
                switch (c)
                {
                case '{':
                case '}':
                case '[':
                case ']':
                case ':':
                    buffer += c;
                    token = { TokenType::Symbol, buffer };
                    tokens.push_back(token);
                    break;
                case ',':
                    break;
                case '\"':
                    //this is a string, capture it completely
                    while (true)
                    {
                        i++;
                        if (i >= string.size())
                        {
                            //maybe throw an error
                            break;
                        }
                        c = string[i];
                        if (c == '\"' && string[i - 1] != '\\')
                        {
                            //this is the closing quote, the string is completed
                            break;
                        }
                        buffer += c;
                    }
                    
                    token = { TokenType::String, buffer };
                    tokens.push_back(token);
                    break;
                case 't':
                    //must be a true value
                    buffer = string.substr(i, 4);
                    if (buffer == "true")
                    {
                        token = { TokenType::Boolean, buffer };
                        tokens.push_back(token);
                    }
                    i += 4;
                    break;
                case 'f':
                    //must be a false value
                    buffer = string.substr(i, 5);
                    if (buffer == "false")
                    {
                        token = { TokenType::Boolean, buffer };
                        tokens.push_back(token);
                    }
                    i += 5;
                    //maybe throw an error
                    break;
                case 'n':
                    //must be a null value
                    buffer = string.substr(i, 4);
                    if (buffer == "null")
                    {
                        token = { TokenType::Null, buffer };
                        tokens.push_back(token);
                    }
                    i += 4;
                    //maybe throw an error
                    break;
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    //must be a number so parse all chars, that are numbers
                    while (true)
                    {
                        c = string[i];
                        if ((c < '0' || c > '9') && c != '-' && c != '.' && c != 'e' && c != 'E')
                        {
                            i--;
                            //this is the final character, the number is completed
                            break;
                        }
                        buffer += c;
                        i++;
                        if (i >= string.size())
                        {
                            //maybe throw an error
                            break;
                        }
                    }
                    token = { TokenType::Number, buffer };
                    tokens.push_back(token);
                }
            }

            return tokens;
        }

        static Value parse(std::vector<Token> &tokens) {
            Value value;
            if(tokens.size() == 0) {
                return value;
            }
            // enum TokenType { Symbol, String, Boolean, Number, Null};
            Token current = tokens[0];
            std::string str;
            if(current.type == TokenType::String) {
                value.SetStringValue(current.value);
                tokens.erase(tokens.begin());
                return value;
            }
            else if(current.type == TokenType::Number) {
                double number = std::stod(current.value);
                value.SetNumberValue(number);
                tokens.erase(tokens.begin());
                return value;
            }
            else if(current.type == TokenType::Null) {
                value.SetNullValue();
                tokens.erase(tokens.begin());
                return value;
            }
            else if(current.type == TokenType::Boolean) {
                value.SetBooleanValue(current.value == "true");
                tokens.erase(tokens.begin());
                return value;
            }
            else if(current.type == TokenType::Symbol) {
                if(current.value == "{") {
                    //it is an object
                    tokens.erase(tokens.begin());
                    std::map<std::string, Value> object;
                    while(tokens[0].value != "}") {
                        if(tokens[0].type == TokenType::String && tokens[1].value == ":") {
                            str = tokens[0].value;
                            tokens.erase(tokens.begin(), tokens.begin() + 2);
                            object[str] = parse(tokens);
                        }
                    }
                    tokens.erase(tokens.begin());
                    value.SetObjectValue(object);
                    return value;
                }
                if(current.value == "[") {
                    //it is an object
                    std::vector<Value> array;
                    tokens.erase(tokens.begin());
                    while(tokens[0].value != "]") {
                        array.push_back(parse(tokens));
                    }
                    tokens.erase(tokens.begin());
                    value.SetArrayValue(array);
                }
            }
            return value;
        }

    public:
        static void PrintJSON(const Value &json) {
          bool booleanValue;
          double numberValue;
          std::string stringValue;
          std::vector<Value> arrayValue;
          std::map<std::string, Value> objectValue;

          switch(json.GetType()) {
            case ValueType::Null:
              std::cout<<"null"<<std::endl;
              break;
            case ValueType::Boolean:
              json.GetBooleanValue(booleanValue);
              std::cout<<(booleanValue ? "true": "false")<<std::endl;
              break;
            case ValueType::Number:
              json.GetNumberValue(numberValue);
              std::cout<<numberValue<<std::endl;
              break;
            case ValueType::String:
              json.GetStringValue(stringValue);
              std::cout<<stringValue<<std::endl;
              break;
            case ValueType::Array:
              json.GetArrayValue(arrayValue);
              std::cout<<"["<<std::endl;
              std::cout<<stringValue<<std::endl;
              for(int i = 0; i < arrayValue.size(); i++) {
                std::cout<<", ";
                PrintJSON(arrayValue[i]);
              }
              std::cout<<"]"<<std::endl;
              break;
            
            case ValueType::Object:
              json.GetObjectValue(objectValue);
              std::cout<<"{"<<std::endl;
              std::cout<<stringValue<<std::endl;
              for (const std::pair<std::string, Value> &element : objectValue) {
                std::cout<<", "<<element.first<<" : ";
                PrintJSON(element.second);
              }
              std::cout<<"}"<<std::endl;
              break;
          }
        }

        static bool ParseString(std::string string, Value &json)
        {
            std::vector<Token> tokens = lex(string);
            if(tokens.size() == 0) {
                return false;
            }
            json = parse(tokens);

            PrintJSON(json);
            
            return true;
        }
    };
}
