#include <iostream>
#include <iomanip>
#include <windows.h>
//function: x * sqrt(x)/log2(x), x > 0

using namespace std;

bool isPositive(double value);
double integrateC(double start, double end, double step);
double integrateAsm(double start, double end, double step);
void enter(double* value, char* msg);


int main(int argc, char** argv) {
	double start, end, step;
	enter(&start, (char*)"enter start");
	enter(&end, (char*)"enter end");
	enter(&step, (char*)"enter step");

	LARGE_INTEGER startTime, endTime, freq;
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&startTime);
	cout << "\nc integrated:\n  result: " << setprecision(40) << integrateC(start, end, step) << endl;
	QueryPerformanceCounter(&endTime);
	cout << "  elapsed time: "
		<< fixed << setprecision(0)
		<< (endTime.QuadPart - startTime.QuadPart) * 1000.0 / freq.QuadPart << " ms"
		<< endl;

	QueryPerformanceCounter(&startTime);
	cout << "\nasm integrated:\n  result: " << setprecision(40) << integrateAsm(start, end, step) << endl;
	QueryPerformanceCounter(&endTime);
	cout << "  elapsed time: "
		<< fixed << setprecision(0)
		<< (endTime.QuadPart - startTime.QuadPart) * 1000.0 / freq.QuadPart << " ms"
		<< endl;

	system("pause > NUL");
	return 0;
}

bool isPositive(double value) {
	return value > 0;
}
double integrateC(double start, double end, double step) {
	if (start <= 0 || end <= 0) {
		cout << "IllegalArgument" << endl;
		return -1;
	}
	if (isPositive(end - start) != isPositive(step)) {
		cout << "IllegalArgument" << endl;
		return -1;
	}
	double result = 0.0;
	double x = start;
	if (isPositive(step)) {
		while (x < end) {
			result += x * sqrt(x) / log2(x) * step;
			x += step;
		}
	} else {
		while (x > end) {
			result += x * sqrt(x) / log2(x) * step;
			x += step;
		}
	}
	return result;
}

double integrateAsm(double start, double end, double step) {
	if (start <= 0 || end <= 0) {
		cout << "IllegalArgument" << endl;
		return -1;
	}
	if (isPositive(end - start) != isPositive(step)) {
		cout << "IllegalArgument" << endl;
		return -1;
	}
	double result = 0.0;
	if (isPositive(step)) {
		double result = 0.0;
		double stub = 0;
		_asm {
			finit
			fld end
			fld start
			loop_start:
				fcom
				fstsw ax
				and ah, 01000001b
				jz loop_end

				fld start
				fsqrt

				fmul start

				fld1; log2(x)
				fld start
				fyl2x
					
				fdiv

				fmul step

				fadd result
				fstp result

				fadd step
				fst start

				jmp loop_start
			loop_end:
			fwait
		}
		return result;
	} else {
		_asm {
			finit
			fld end
			fld start
			loop_start_reverse :
				fcom
				fstsw ax; регистр сопроцессора или цп
				not ah
				and ah, 00000001b; куда сохраняет результат, что означает 01000101
				jz loop_end_reverse

				fld start
				fsqrt

				fmul start

				fld1; log
				fld start
				fyl2x

				fdiv

				fmul step

				fadd result
				fstp result

				fadd step
				fst start

				jmp loop_start_reverse
				loop_end_reverse :
			fwait
		}
	}
	return result;
}

void enter(double* value, char* msg) {
	cout << msg << endl;
	do {
		fflush(stdin);
	} while (scanf_s("%lf", value) != 1);
}