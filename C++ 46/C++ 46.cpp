#include <iostream>
#include <string>
#include <windows.h>

using namespace std;



#pragma region Exceptions

class Exception : public exception
{
protected:
	mutable string c_message;
	string time, source, text;
	int line;

public:
	Exception() : time(), source(), text("My exception"), line() {}

	Exception(string _time, string _source, string _text, int _line) : exception(_text.c_str())
	{
		time = _time;
		source = _source;
		text = _text;
		line = _line;
	}

	const char* what() const override = 0
	{
		string message = "Time : " + time
			+ "\nSource : " + source
			+ "\nText : " + text
			+ "\nLine : " + to_string(line);

		c_message = message;
		return c_message.c_str();
	}


}; // text, line, source, time. //cari vaxti alinmasini internetden arashdirin
class DatabaseException : public Exception
{
public:

	DatabaseException() : Exception() {}

	DatabaseException(string _time, string _source, string _text, int _line)
		:Exception(_time, _source, _text, _line) {}

	const char* what() const override
	{
		return Exception::what();
	}

};


class InvalidArgumentException :public Exception 
{
public:
	InvalidArgumentException() : Exception() {}

	InvalidArgumentException(string _time, string _source, string _text, int _line)
		:Exception(_time, _source, _text, _line) {}

	const char* what() const override
	{
		return Exception::what();
	}
};

#pragma endregion


#pragma region UserClass
class User {
	static int s_id;
	int id;
	string username, password, name, surname;

public:

	User() : id(), username(), password(), name(), surname() {}

	User(string _username, string _password, string _name, string _surname)
	{
		id = ++s_id;
		username = _username;
		password = _password;
		name = _name;
		surname = _surname;
	}


	///GETTERS
	const string getUsername() const { return username; }
	const string getName() const { return name; }
	const string getSurname() const { return surname; }
	const string getPassword() const { return password; }
	const int getId() const{ return id; }
	//////////////////////////////////////////


	void show() const
	{
		cout << "ID : " << id << endl;
		cout << "Username : " << username<< endl;
		cout << "Password : " << password << endl;
		cout << "Name : " << name << endl;
		cout << "Surname : " << surname << endl;
	}
};

int User::s_id = 0;

#pragma endregion


#pragma region DataBase

class Database {
	User** users;
	size_t user_count = 0;

public:

	Database() : users(), user_count() {}

	Database(User** _users, int _user_count)
	{
		if (users != nullptr)
			delete[] users;

		users = _users;
		user_count = _user_count;
	}


	User** addUser(User* user)
	{
		User** temp = new User * [user_count + 1] {};

		for (size_t i = 0; i < user_count; i++)
			temp[i] = users[i];

		temp[user_count] = user;
		delete[] users;
		users = temp;
		user_count++;
		return users;
	}


	User* getUserByUsername(string username)
	{
		for (size_t i = 0; i < user_count; i++)
			if (users[i]->getUsername() == username)
				return users[i];
		return NULL;
		
	}


	void updateUser(User* olduser, User* newuser)
	{
		for (size_t i = 0; i < user_count; i++)
			if (users[i]->getUsername() == olduser->getUsername())
				users[i] = newuser;				

		throw DatabaseException(__TIME__, __FILE__, "There is no such user. Please, try another username", __LINE__);
	}


	void showUsers()
	{
		for (size_t i = 0; i < user_count; i++)
			users[i]->show();
	}


	size_t getCount() { return user_count; }


	void deleteUserById(const int& id)
	{
		bool flag = false;
		auto temp = new User * [user_count - 1] {};

		for (size_t i = 0, k = 0; i < user_count - 1; i++)
		{
			if (users[i]->getId() != id)
				temp[k++] = users[i];
			else
				flag = true;
		}

		if (users[user_count - 1]->getId() != id && !flag)
			throw DatabaseException(__TIME__, __FILE__, "There is no such user. Please, try another username", __LINE__);

		delete[] users;
		users = temp;
		--user_count;
	}

};

#pragma endregion



#pragma region Registration
class Registration {
	Database _database;
	User* user;
public:

	Registration(const Database& database)
	{
		_database = database;
	}


	//eger bu username istifadechi yoxdursa error atsin
	//eger username varsa amma password yanlishdirsa error atsin
	void signIn(string username, string password)
	{
		User* user = _database.getUserByUsername(username);
		if(!user)
			throw DatabaseException(__TIME__, __FILE__, "There is no such user. Please, try another username", __LINE__);
		else
		{
			if (user->getPassword() != password)
				throw DatabaseException(__TIME__, __FILE__, "Wrong password...", __LINE__);
			else
				cout << "Welcome to your account...)";
		}
	}


	//Eger istifadechi varsa hemen username de throw DatabaseException
	//Eger username xarakter sayi 6 dan kichikdirse InvalidArgumentException
	//Eger username ilk herf kichikdirse InvalidArgumentException
	//Eger password xarakter sayi 6 dan kichikdirse InvalidArgumentException
	//Eger name xarakter sayi 3 dan kichikdirse InvalidArgumentException
	//Eger surname xarakter sayi 4 dan kichikdirse InvalidArgumentException
	void signUp(string username, string password, string name, string surname)
	{
		if(username.size() < 6)
			throw InvalidArgumentException(__TIME__, __FILE__, "Username is less than 6 characters\n", __LINE__);
		if(!isupper(username[0]))
			throw InvalidArgumentException(__TIME__, __FILE__, "Username first letter is lowercase..\n", __LINE__);
		if(password.size() < 6)
			throw InvalidArgumentException(__TIME__, __FILE__, "Password is less than 6 characters\n", __LINE__);
		if(name.size() < 3)
			throw InvalidArgumentException(__TIME__, __FILE__, "Name is less than 3 characters\n", __LINE__);
		if(surname.size() < 4)
			throw InvalidArgumentException(__TIME__, __FILE__, "Surname is less than 4 characters\n", __LINE__);
		if (_database.getUserByUsername(username) != NULL)
			throw DatabaseException(__TIME__, __FILE__, "Such account already is exist..\n", __LINE__);

		User* user = new User(username, password, name, surname);
		_database.addUser(user);
	}

	Database getDataBase()
	{
		return _database;
	}

};
#pragma endregion


#pragma region STARTUP
class StartUp 
{
public:
	static void Start() 
	{
		User u1("asd1", "12345|1", "ddd1", "sss1");
		User u2("asd2", "12345|2", "ddd2", "sss2");
		User u3("asd3", "12345|3", "ddd3", "sss3");
		User u4("asd3", "12345|4", "ddd4", "sss4");
		User** us = new User * [] {&u1, & u2, & u3};

		Database dt(us, 3);
		Registration twitter(dt);
		try
		{
			dt.showUsers();

			twitter.signUp("Asd123", "12345|", "ddd1", "sss1");
			cout << endl << endl;
			twitter.getDataBase().showUsers();
		}
		catch (Exception& t)
		{
			cout << t.what();
		}
		// twitter.getDatabase().addUser(user);

		//mainde yazilacaq her shey burda yazilsin.
	}
};

#pragma endregion



int main() 
{
	class StartUp s;
	s.Start();
	
}


