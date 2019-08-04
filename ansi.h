#ifndef ANSI_H_
#define ANSI_H_

#include <iostream>
#include <map>
#include <string>
#include <unordered_set>

namespace ansi {
	enum color {
		normal, red, orange, yellow, yeen, green, blue, cyan, magenta, purple, black, gray, lightgray, white, pink, sky
	};

	enum style {bold, dim, italic, underline};
	enum color_type {text, background};
	enum action {reset, end_line, check, nope, warning, information, open_paren, close_paren, enable_parens};

	template <typename T>
	struct ansi_pair {
		T value;
		bool add;
		ansi_pair(T value, bool add): value(value), add(add) {}
	};

	struct color_pair {
		ansi::color color;
		ansi::color_type type;

		color_pair(ansi::color color, ansi::color_type type): color(color), type(type) {}
		color_pair(ansi::color color): color(color), type(text) {}

		std::string left() const;
		std::string right() const;
	};

	class ansistream {
		private:
			ansi::color text_color;
			ansi::color bg_color;
			std::unordered_set<ansi::style> styles;
			bool parens_on = false;
			ansistream & left_paren();
			ansistream & right_paren();

		public:
			std::ostream &content_out;
			std::ostream &style_out;

			ansistream();
			ansistream(std::ostream &stream): content_out(stream), style_out(stream) {}
			ansistream(std::ostream &c, std::ostream &s): content_out(c), style_out(s) {}
			static ansistream & err();
			ansistream & flush();
			ansistream & clear();

			/** Moves the cursor to a given position. Arguments are expected to be zero-based. */
			ansistream & jump(int x, int y);
			/** Moves the cursor to the top-left corner. */
			ansistream & jump();

			/** Saves the cursor position via CSI s. */
			ansistream & save();
			/** Restores the cursor position via CSI u. */
			ansistream & restore();

			/** Clears the entire line at the cursor. */
			ansistream & clear_line();
			/** Clears all text to the left of the cursor. */
			ansistream & clear_left();
			/** Clears all text to the right of the cursor. */
			ansistream & clear_right();

			/* These functions move the cursor in one direction by a given offset. */ /**/
			ansistream & up(size_t = 1);
			ansistream & down(size_t = 1);
			ansistream & right(size_t = 1);
			ansistream & left(size_t = 1);

			/** Sets the absolute vertical position of the cursor. Zero-based. */
			ansistream & vpos(size_t);
			/** Sets the absolute horizontal position of the cursor. Zero-based. */
			ansistream & hpos(size_t);

			/** Scrolls up by a number of lines. Lines at the bottom are replaced with blank lines. */
			ansistream & scroll_up(size_t);
			/** Scrolls down by a number of lines. Lines at the top are replaced with blank lines. */
			ansistream & scroll_down(size_t);

			/** Deletes a number of characters starting at the cursor. */
			ansistream & delete_chars(size_t = 1);
			
			/** Enables origin mode: the home position is set to the top-left corner of the margins. */
			ansistream & set_origin();
			/** Disables origin mode. */
			ansistream & reset_origin();

			/** Sets the horizontal margins of the scrollable area. Zero-based. */
			ansistream & hmargins(size_t left, size_t right);
			/** Resets the horizontal margins of the scrollable area. */
			ansistream & hmargins();
			/** Enables horizontal margins. This must be called before calling hmargins. */
			ansistream & enable_hmargins();
			/** Disables horizontal margins. */
			ansistream & disable_hmargins();
			/** Sets the vertical margins of the scrollable area. Zero-based. */
			ansistream & vmargins(size_t top, size_t bottom);
			/** Resets the vertical margins of the scrollable area. */
			ansistream & vmargins();
			/** Sets the vertical and horizontal margins of the scrollable area. Zero-based. */
			ansistream & margins(size_t top, size_t bottom, size_t left, size_t right);
			/** Resets the vertical and horizontal margins of the scrollable area. */
			ansistream & margins();

			ansistream & operator<<(const ansi::color &);
			ansistream & operator<<(const ansi::color_pair &);
			ansistream & operator<<(const ansi::style &);
			ansistream & operator<<(const ansi::ansi_pair<ansi::style> &);
			ansistream & operator<<(const ansi::action &);
			ansistream & operator<<(std::ostream & (*fn)(std::ostream &));
			ansistream & operator<<(std::ostream & (*fn)(std::ios &));
			ansistream & operator<<(std::ostream & (*fn)(std::ios_base &));
			ansistream & operator<<(const char *t);

			template <typename T>
			ansistream & operator<<(const T &value) {
				// Piping miscellaneous values into the ansistream simply forwards them as-is to the content stream.
				left_paren();
				content_out << value;
				right_paren();
				return *this;
			}

			ansistream & operator>>(const ansi::style &);
	};

	extern ansistream out;
	static action endl   = action::end_line;
	static action good   = action::check;
	static action bad    = action::nope;
	static action warn   = action::warning;
	static action info   = action::information;
	static action oparen = action::open_paren;
	static action cparen = action::close_paren;
	static action parens = action::enable_parens;

	std::string get_text(const ansi::color &);
	std::string get_bg(const ansi::color &);
	color_pair fg(ansi::color color);
	color_pair bg(ansi::color color);
	ansi_pair<ansi::style> remove(ansi::style);
	std::string wrap(const std::string &, const color_pair &);
	std::string wrap(const std::string &, const color &);
	std::string wrap(const std::string &, const style &);
	void write(std::ostream &, const std::string &);
	void write(const std::string &);

	/** Strips the ANSI escape sequences from a string. */
	std::string strip(const std::string &);

	const std::string reset_all = "\e[0m";
	const std::string reset_fg  = "\e[39m";
	const std::string reset_bg  = "\e[49m";

	const std::string str_check   = "\u2714";
	const std::string str_nope    = "\u2718";
	const std::string str_warning = "\u26a0\ufe0f";

	const std::map<color, std::string> color_bases = {
		{normal,    "9"},
		{red,       "1"},
		{orange,    "8;5;202"},
		{yellow,    "3"},
		{yeen,      "8;5;112"},
		{green,     "2"},
		{blue,      "4"},
		{cyan,      "6"},
		{magenta,   "5"},
		{purple,    "8;5;56"},
		{black,     "0"},
		{gray,      "8;5;8"},
		{lightgray, "8;5;7"},
		{white,     "7"},
		{pink,      "8;5;219"},
		{sky,       "8;5;153"},
	};

	const std::map<style, std::string> style_codes = {
		{bold,      "\e[1m"},
		{dim,       "\e[2m"},
		{italic,    "\e[3m"},
		{underline, "\e[4m"},
	};

	const std::map<style, std::string> style_resets = {
		{bold,      "\e[22m"},
		{dim,       "\e[22m"},
		{italic,    "\e[23m"},
		{underline, "\e[24m"},
	};
}

std::string operator"" _b(const char *str, unsigned long);
std::string operator"" _d(const char *str, unsigned long);
std::string operator"" _i(const char *str, unsigned long);
std::string operator"" _u(const char *str, unsigned long);
std::string operator"" _bd(const char *str, unsigned long);

#endif
