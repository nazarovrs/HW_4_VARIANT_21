#include <iostream>
#include <chrono>
#include <windows.h>
#include <random>
#include <omp.h>

enum Skill { BAD, OK, NORMAL, GOOD };
const int COUNT_OF_STUDENTS = 10;

std::random_device rd; 
std::mt19937 gen(rd()); 

struct Student {
	int id = 0;
	bool isReady = false;
	Skill skill = OK;
	void Prepare() {
#pragma omp critical
		std::cout << "\n" << id << " студент начал подготовку.";
		int temp = std::uniform_int_distribution<>(0, 3)(gen);
		Sleep(std::uniform_int_distribution<>(200,20000)(gen));
		switch (temp) {
		case 0:
			skill = BAD;
			break;
		case 1:
			skill = OK;
			break;
		case 2:
			skill = NORMAL;
			break;
		case 3:
			skill = GOOD;
			break;
		}
#pragma omp critical
		std::cout << "\nНу, я готов. (Студент номер " << id << " тянет руку) ";
		isReady = true;
	}
};

struct Teacher {
	void check(Student student);
};
void Teacher::check(Student student) {
#pragma omp critical
	std::cout << "\n\tСтудент " << student.id << " отвечает... ";
#pragma omp critical 
	{
	Sleep(std::uniform_int_distribution<>(500, 3000)(gen));
	switch (student.skill)
	{
	case BAD:
		if ((std::rand() % 4) >= 1)
			std::cout << "\n\t\t" << "Студент №" << student.id << ", натянем троечку... Следующий!";
		else
			std::cout << "\n\t\t" << "Студент №" << student.id << ", вам неуд, следующий!";
		break;
	case OK:
		std::cout << "\n\t\t" << "Студент №" << student.id << ", вам \"удовлетворительно\", следующий!";
		break;
	case NORMAL:
		std::cout << "\n\t\t" << "Студент №" << student.id << ", ваша оценка \"Хорошо\", следующий!";
		break;
	case GOOD:
		std::cout << "\n\t\t" << "Студент №" << student.id << ", \"Отлично\", приятно было слушать, следующий!";
		break;
	default:
		std::cout << "\n\t\t" << "Студент №" << student.id << ", вы точно на мои лекции ходили? совсем не по теме. Однозначно пересдача, следующий!";
		break;
	}
	}
}
Student* students;
Teacher* teach;



void waiting() {
	int count = 0;
	while (1) {
		for (int i = 0; i < COUNT_OF_STUDENTS; i++) {
			if (students[i].isReady == true) {
				teach->check(students[i]);
				students[i].isReady = false;
				count++;
			}
		}
		if (count == COUNT_OF_STUDENTS)
			break;
	}
}

int main() {
	setlocale(LC_ALL, "russian");
	students = new Student[COUNT_OF_STUDENTS]();
	teach = new Teacher();
#ifdef _OPENMP
	{
#pragma omp parallel num_threads(COUNT_OF_STUDENTS)
		{
#pragma omp for nowait
			for (int i = 0; i < COUNT_OF_STUDENTS; i++) {
				students[i].id = i;
				students[i].Prepare();
			}
#pragma omp single //
			waiting();
		}
	}
#else {
	std::cout << "_OPENMP не поддерживается, программа будет выполнена в одном потоке.";
	for (int i = 0; i < COUNT_OF_STUDENTS; i++) {
		students[i].id = i;
		students[i].Prepare();
	}
	int count = 0;
	while (1) {
		for (int i = 0; i < COUNT_OF_STUDENTS; i++) {
			if (students[i].isReady == true) {
				teach->check(students[i]);
				students[i].isReady = false;
				count++;
			}
		}
		if (count == COUNT_OF_STUDENTS)
			break;
	}
#endif
	}