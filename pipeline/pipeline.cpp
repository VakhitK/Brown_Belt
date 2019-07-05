/*
 * pipeline.cpp
 *
 *  Created on: 5th of July 2019.
 *      Author: vakhit.k
 */
#include "test_runner.h"
#include <functional>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <utility>
#include <vector>

using namespace std;

struct Email {
	string from;
	string to;
	string body;
};

class Worker {
public:
	virtual ~Worker() = default;
	virtual void Process(unique_ptr<Email> email) = 0;
	virtual void Run() {
		// только первому worker-у в пайплайне нужно это имплементировать
		throw logic_error("Unimplemented");
	}

protected:
	// реализации должны вызывать PassOn, чтобы передать объект дальше
	// по цепочке обработчиков
	void PassOn(unique_ptr<Email> email) const {
		if (next_) {
			next_->Process(move(email));
		}
	}

public:
	void SetNext(unique_ptr<Worker> next) {
		next_ = move(next);
	}

private:
	unique_ptr<Worker> next_;
};

class Reader: public Worker {
public:
	explicit Reader(istream& in) :
			in_(in) {
	}

	void Process(unique_ptr<Email> /* email */) override {
		// не делаем ничего
	}

	void Run() override {
		for (;;) {
			auto email = make_unique<Email>();
			getline(in_, email->from);
			getline(in_, email->to);
			getline(in_, email->body);
			if (in_) {
				PassOn(move(email));
			} else {
				// в реальных программах здесь стоит раздельно проверять badbit и eof
				break;
			}
		}
	}

private:
	istream& in_;
};

class Filter: public Worker {
public:
	using Function = function<bool(const Email&)>;

public:
	explicit Filter(Function func) :
			func_(move(func)) {
	}

	void Process(unique_ptr<Email> email) override {
		if (func_(*email)) {
			PassOn(move(email));
		}
	}

private:
	Function func_;
};

class Copier: public Worker {
public:
	explicit Copier(string to) :
			to_(move(to)) {
	}

	void Process(unique_ptr<Email> email) override {
		if (email->to != to_) {
			auto copy = make_unique<Email>(*email);
			copy->to = to_;
			PassOn(move(email));
			PassOn(move(copy));
		} else {
			PassOn(move(email));
		}
	}

private:
	string to_;
};

class Sender: public Worker {
public:
	explicit Sender(ostream& out) :
			out_(out) {
	}

	void Process(unique_ptr<Email> email) override {
		out_ << email->from << endl << email->to << endl << email->body << endl;
		PassOn(move(email));  // never forget
	}
private:
	ostream& out_;
};

class PipelineBuilder {
public:
	explicit PipelineBuilder(istream& in) {
		workers_.push_back(make_unique<Reader>(in));
	}

	PipelineBuilder& FilterBy(Filter::Function filter) {
		workers_.push_back(make_unique<Filter>(move(filter)));
		return *this;
	}

	PipelineBuilder& CopyTo(string recipient) {
		workers_.push_back(make_unique<Copier>(move(recipient)));
		return *this;
	}

	PipelineBuilder& Send(ostream& out) {
		workers_.push_back(make_unique<Sender>(out));
		return *this;
	}

	unique_ptr<Worker> Build() {
		for (size_t i = workers_.size() - 1; i > 0; --i) {
			workers_[i - 1]->SetNext(move(workers_[i]));
		}
		return move(workers_[0]);
	}

private:
	vector<unique_ptr<Worker>> workers_;
};

void TestSanity() {
	string input = ("erich@example.com\n"
			"richard@example.com\n"
			"Hello there\n"

			"erich@example.com\n"
			"ralph@example.com\n"
			"Are you sure you pressed the right button?\n"

			"ralph@example.com\n"
			"erich@example.com\n"
			"I do not make mistakes of that kind\n");
	istringstream inStream(input);
	ostringstream outStream;

	PipelineBuilder builder(inStream);
	builder.FilterBy(
			[](const Email& email) {return email.from == "erich@example.com";});
	builder.CopyTo("richard@example.com");
	builder.Send(outStream);
	auto pipeline = builder.Build();

	pipeline->Run();

	string expectedOutput = ("erich@example.com\n"
			"richard@example.com\n"
			"Hello there\n"

			"erich@example.com\n"
			"ralph@example.com\n"
			"Are you sure you pressed the right button?\n"

			"erich@example.com\n"
			"richard@example.com\n"
			"Are you sure you pressed the right button?\n");

	ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSanity);

	int i;
	cin >> i;

	return 0;
}
