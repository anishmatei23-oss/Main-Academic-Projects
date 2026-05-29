#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Person {
private:
    string name;
    int day;
    int month;
    int year;

public:
    Person(const string& name, int day, int month, int year)
        : name(name), day(day), month(month), year(year) {}

    string getName() const {
        return name;
    }

    string getZodiacSign() const {
        if (month < 1 || month > 12 || day < 1 || day > 31) {
            return "Invalid Date";
        }

        if ((month == 1 && day >= 20) || (month == 2 && day <= 18)) return "Aquarius";
        if ((month == 2 && day >= 19) || (month == 3 && day <= 20)) return "Pisces";
        if ((month == 3 && day >= 21) || (month == 4 && day <= 19)) return "Aries";
        if ((month == 4 && day >= 20) || (month == 5 && day <= 20)) return "Taurus";
        if ((month == 5 && day >= 21) || (month == 6 && day <= 20)) return "Gemini";
        if ((month == 6 && day >= 21) || (month == 7 && day <= 22)) return "Cancer";
        if ((month == 7 && day >= 23) || (month == 8 && day <= 22)) return "Leo";
        if ((month == 8 && day >= 23) || (month == 9 && day <= 22)) return "Virgo";
        if ((month == 9 && day >= 23) || (month == 10 && day <= 22)) return "Libra";
        if ((month == 10 && day >= 23) || (month == 11 && day <= 21)) return "Scorpio";
        if ((month == 11 && day >= 22) || (month == 12 && day <= 21)) return "Sagittarius";
        if ((month == 12 && day >= 22) || (month == 1 && day <= 19)) return "Capricorn";
        return "Unknown";
    }
};

class FortuneTeller {
public:
    static const char* getZodiacFact(const string& zodiacSign) {
        static const struct { const char* sign; const char* fact; } facts[] = {
            {"Aquarius", "Aquarius is innovative and values freedom."},
            {"Pisces", "Pisces are deeply empathetic and imaginative."},
            {"Aries", "Aries are bold leaders with adventurous spirits."},
            {"Taurus", "Taurus is patient and appreciates comfort."},
            {"Gemini", "Geminis are adaptable and curious."},
            {"Cancer", "Cancers are nurturing and intuitive."},
            {"Leo", "Leos are confident and love being in the spotlight."},
            {"Virgo", "Virgos are detail-oriented and practical."},
            {"Libra", "Libras seek balance and harmony."},
            {"Scorpio", "Scorpios are passionate and resourceful."},
            {"Sagittarius", "Sagittarians are adventurous and optimistic."},
            {"Capricorn", "Capricorns are disciplined and ambitious."}
        };

        for (const auto& entry : facts) {
            if (entry.sign == zodiacSign) {
                return entry.fact;
            }
        }
        return "No fact available.";
    }

    static void getLuckyNumbers(const string& zodiacSign) {
        static const struct { const char* sign; int numbers[3]; } luckyNumbers[] = {
            {"Aquarius", {4, 8, 13}},
            {"Pisces", {3, 7, 12}},
            {"Aries", {1, 9, 19}},
            {"Taurus", {2, 4, 8}},
            {"Gemini", {5, 9, 10}},
            {"Cancer", {2, 7, 11}},
            {"Leo", {1, 5, 9}},
            {"Virgo", {3, 6, 9}},
            {"Libra", {6, 15, 24}},
            {"Scorpio", {9, 18, 27}},
            {"Sagittarius", {3, 5, 8}},
            {"Capricorn", {2, 8, 10}}
        };

        for (const auto& entry : luckyNumbers) {
            if (entry.sign == zodiacSign) {
                for (int num : entry.numbers) {
                    cout << num << " ";
                }
                cout << endl;
                return;
            }
        }
        cout << "No lucky numbers available.\n";
    }

    static void getFortunes(const string& zodiacSign) {
        static const struct { const char* sign; vector<string> fortunes; } fortunes[] = {
            {"Aquarius", {"A bright future awaits you.", "You will find inspiration soon.", "New opportunities are near."}},
            {"Pisces", {"Trust your instincts.", "A creative project will succeed.", "Good news is on the way."}},
            {"Aries", {"Adventure is coming.", "Your energy will attract success.", "Be ready to take bold steps."}},
            {"Taurus", {"Patience will be rewarded.", "Stay focused on your goals.", "A stable future lies ahead."}},
            {"Gemini", {"New connections will benefit you.", "Communication will bring success.", "Expect pleasant surprises."}},
            {"Cancer", {"Your empathy will guide you.", "Nurture your relationships.", "Good luck in family matters."}},
            {"Leo", {"Your confidence will shine.", "Leadership opportunities are coming.", "Be bold and take risks."}},
            {"Virgo", {"Your attention to detail will pay off.", "Stay organized for success.", "Practical solutions are key."}},
            {"Libra", {"Balance will bring peace.", "Your charm will open doors.", "Seek harmony in all things."}},
            {"Scorpio", {"Your passion will drive success.", "Embrace change for growth.", "Trust your resilience."}},
            {"Sagittarius", {"Adventure awaits.", "Your optimism will inspire others.", "Explore new opportunities."}},
            {"Capricorn", {"Hard work will pay off.", "Focus on long-term goals.", "Your determination is key."}}
        };

        for (const auto& entry : fortunes) {
            if (entry.sign == zodiacSign) {
                for (const string& fortune : entry.fortunes) {
                    cout << "- " << fortune << endl;
                }
                return;
            }
        }
        cout << "No fortunes available.\n";
    }

    static const char* getLoveCompatibility(const string& zodiacSign) {
        static const struct { const char* sign; const char* compatibleWith; } compatibility[] = {
            {"Aquarius", "Gemini, Libra"},
            {"Pisces", "Cancer, Scorpio"},
            {"Aries", "Leo, Sagittarius"},
            {"Taurus", "Virgo, Capricorn"},
            {"Gemini", "Libra, Aquarius"},
            {"Cancer", "Scorpio, Pisces"},
            {"Leo", "Aries, Sagittarius"},
            {"Virgo", "Taurus, Capricorn"},
            {"Libra", "Gemini, Aquarius"},
            {"Scorpio", "Cancer, Pisces"},
            {"Sagittarius", "Aries, Leo"},
            {"Capricorn", "Taurus, Virgo"}
        };

        for (const auto& entry : compatibility) {
            if (entry.sign == zodiacSign) {
                return entry.compatibleWith;
            }
        }
        return "Compatibility not available.";
    }

    static const char* getLuckyColor(const string& zodiacSign) {
        static const struct { const char* sign; const char* color; } luckyColors[] = {
            {"Aquarius", "Blue"},
            {"Pisces", "Sea Green"},
            {"Aries", "Red"},
            {"Taurus", "Green"},
            {"Gemini", "Yellow"},
            {"Cancer", "White"},
            {"Leo", "Gold"},
            {"Virgo", "Brown"},
            {"Libra", "Pink"},
            {"Scorpio", "Black"},
            {"Sagittarius", "Purple"},
            {"Capricorn", "Dark Brown"}
        };

        for (const auto& entry : luckyColors) {
            if (entry.sign == zodiacSign) {
                return entry.color;
            }
        }
        return "No lucky color available.";
    }
};

int main() {
    string name;
    int day, month, year;

    cout << "Enter your name: ";
    getline(cin, name);

    cout << "Enter your birth date (day month year): ";
    cin >> day >> month >> year;

    Person person(name, day, month, year);
    string zodiacSign = person.getZodiacSign();

    cout << "\nWelcome, " << person.getName() << "! Your zodiac sign is " << zodiacSign << ".\n";

    int choice;
    do {
        cout << "\n1. Fun Fact\n";
        cout << "2. Fortunes\n";
        cout << "3. Lucky Numbers\n";
        cout << "4. Lucky Color\n";
        cout << "5. Love Compatibility\n";
        cout << "6. Exit\n";
        cout << "Enter your choice (1-6): ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "\nFun fact: " << FortuneTeller::getZodiacFact(zodiacSign) << endl;
                break;
            case 2:
                cout << "\nHere are your fortunes:\n";
                FortuneTeller::getFortunes(zodiacSign);
                break;
            case 3:
                cout << "\nYour lucky numbers are: ";
                FortuneTeller::getLuckyNumbers(zodiacSign);
                break;
            case 4:
                cout << "\nYour lucky color is: " << FortuneTeller::getLuckyColor(zodiacSign) << "\n";
                break;
            case 5:
                cout << "\nYour love compatibility is: " << FortuneTeller::getLoveCompatibility(zodiacSign) << "\n";
                break;
            case 6:
                cout << "\nGoodbye!\n";
                break;
            default:
                cout << "\nInvalid choice. Please try again.\n";
        }
    } while (choice != 6);

    return 0;
}
