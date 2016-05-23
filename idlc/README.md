# IDLC(Interface Definition Language Compiler)

- 'idlc' is a kind of interface language compiler. 
- .idl file It will compile .idl files and generate translated code as specified(eg. C++/Python/C#..) which is used for serializing and de-serializing

## Quick Example

1. write below "example.idl" and save it.

```
message Person : 1 
{
	int32 id; 
	string name; 
	string email; 
};
```

2. To translate this file, compile "example.idl" file with '''idlc'''. In this example, we will translate it from .idl to .cpp.

> idlc -lcpp example.idl

3. result

```
// example.h 
struct Person 
{ 
	enum { MSG_ID = 1; } 
	int32_t id; 
	std::string name; 
	std::string email;
	
	int Size() const 
	{
		// generated code stub to get size of this "Person" structure
	}
	bool Store(std::vector& buf)
	{
		// generated code stub to serialize this "Person" structure and store data stream to "buf"
	}
	bool Store(char** buf)
	{
		// generated code stub to serialize this "Person" structure and store data stream to "buf"
	}
	bool Load(const std::vector& buf)
	{
		// generated code stub to read data stream from "buf and de-serialize it
	}
	bool Load(const char** buf)
	{
		// generated code stub to read data stream from "buf and de-serialize it
	}
}; // end of struct ‘Person’ 
```

4. Use it in your project

```
include "example.h"

int main() 
{ 
	Person person1_; 
	person1_.id = 123; 
	person1_.name = "somename"; 
	person1_.email = "somename@email.com";
	std::vector<char> buf;
	person1_.Store(buf); // person1_ 객체를 시리얼라이즈하여 buf 저장 합니다.
	
	Person person2_; // 값이 들어 있지 않은 새로운 객체 생성
	person2_.Load(buf); // buf의 데이터를 person2_객체에 디-시리얼라이즈 합니다.
	
	std::cout << "person id : " << person2_.id << std::endl;
	std::cout << "person name : " << person2_.name << std::endl;
	std::cout << "person email : " << person2_.email << std::endl;

	return 0;
} 
```

## Keywords

- "message" : numbered and structured data to be serialized/de-serialized.
- "struct" : pure structured data to be serialized/de-serialized.
- "enum" : enumerator.
- ".cpp|.cs|.py %% ~ %%" : literal block for each language.
- "typedef" : re-definition of type name

## Support data types

| Category     | Type name | Data size      | Serialize result | 
|--------------|-----------|----------------|------------------| 
|Primitive type|byte       |signed 1 byte   |signed 1 byte     | 
|              |boolean    |unsigned 1 byte |unsigned 1 byte   | 
|              |int16      |signed 2 bytes  |signed 2 bytes    | 
|              |int32      |signed 4 bytes  |signed 4 bytes    | 
|              |int64      |signed 8 bytes  |signed 8 bytes    | 
|              |uint16     |unsigned 2 bytes|unsigned 2 bytes  | 
|              |uint32     |unsigned 4 bytes|unsigned 4 bytes  | 
|              |uint64     |unsigned 8 bytes|unsigned 8 bytes  | 
|              |float      |float 4 bytes   |float 4 bytes     | 
|              |double     |float 8 bytes   |float 8 bytes     | 
|string type   |string     |dynamic         |signed 4 bytes(for 'length') + sizeof(char)length|
|container type|list<T>    |dynamic         |signed 4 bytes(element count) + sizeof(T) 'element count'| 
|              |array<T>   |dynamic         |signed 4 bytes(element count) + sizeof(T) 'element count'| 
|              |array      |static          |sizeof(T) 'element count'| 
|              |map        |dynamic         |signed 4 bytes(element count) + sizeof(K) 'element count' + sizeof(E) 'element count'|

## Grammar(BNF)

```
<stmt_list> 	::= <stmt> | <stmt_list> <stmt>
<stmt>		::= <message> | <typedef> | <enum> | <literal> | <struct>
<literal>	::= ".cpp|.java|.py" %% ~ %%
<typedef> 	::= "typedef" <var_decl>

<message>	::= "message" <var_name> : <integer> { <var_decl_list> }; | 
		    "message" <var_name> : <integer> { } ; | 
		    "message" <var_name> ( <var_name> ) : <integer> { <var_decl_list> } ; | 
		    "message" <var_name> ( <var_name> ) : <integer> { } ; 

<var_decl_list> ::= <var_decl> | 
		    <var_decl_list> <var_decl> <var_decl> : <var_type> <var_name> ;

<struct>	::= "struct" var_name { var_decl_list } ; | 
		    "struct" var_name { } ; | 
		    "struct" var_name ( var_name ) { var_decl_list } ; |
		    "struct" var_name ( var_name ) { };

// -- enum -- // 
<enum> 		::= "enum" <var_name> { } ; | 
		    "enum" <var_name> { <enum_decl_list> } ; 
		    
<enum_decl_list>::= <enum_decl> | 
		    <enum_decl_list> , enum_decl 
		    
<enum_decl>	::= <var_name> = <integer>;

// -- variable -- // 
<var_decl>	::= <var_type> <var_name>; 
		    
<var_type> 	::= "boolean" | "byte" | 
		    "int16" | "int32" | "int64" | "uint16" | "uint32" | "uint64" | 
		    "double" | "float" | 
		    "string" | 
		    "array" < <var_type> > | 
		    "list" < <var_type> > | 
		    "map" < <var_type>, <var_type> > | 
		    <var_name> 

<var_name> 	::= [A-Za-z_][A-Za-z0-9_]* 
<integer>	::= [0-9]* 
```
