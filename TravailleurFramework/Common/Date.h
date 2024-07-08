#ifndef Date_H
#define Date_H

#include <chrono>
#include <string>

class Date
{
	public:
		Date();

		int getDayOfMonth() const;
		int getDayOfWeek() const;
		int getMonth() const;
		int getYear() const;
		std::string getDateFormatMMDDYYYY() const;
		void advanceDays(const int nDays);
		void goBackDays(const int nDays);
	private:
		std::chrono::year_month_day today;
};

#endif