#include <iostream>
#include <stdexcept>
#include <string>

#include "ansi.h"

using std::string;

namespace ansi {
	ansistream out = {std::cout, std::cerr};

	color_pair fg(ansi::color color) {
		return {color, text};
	}

	color_pair bg(ansi::color color) {
		return {color, background};
	}

	string get_text(const ansi::color &color) {
		return "\e[3" + color_bases.at(color) + "m";
	}

	string get_bg(const ansi::color &color) {
		return "\e[4" + color_bases.at(color) + "m";
	}

	ansi_pair<style> remove(ansi::style style) {
		return {style, false};
	}

	string wrap(const string &str, const ansi::color_pair &pair) {
		return pair.left() + str + pair.right();
	}

	string wrap(const string &str, const ansi::color &color) {
		return wrap(str, color_pair(color));
	}

	string wrap(const string &str, const ansi::style &style) {
		return style_codes.at(style) + str + style_resets.at(style);
	}

	void write(std::ostream &os, const std::string &str) {
		os << str;
		os.flush();
	}

	void write(const std::string &str) {
		write(std::cout, str);
	}

	std::string strip(const std::string &str) {
		std::string out = "";
		size_t len = str.length();
		char c;
		for (size_t i = 0; i < len; ++i) {
			c = str[i];
			if (c != '\x1b') {
				out.push_back(c);
				continue;
			}

			for (; str[i] < 0x40 || 0x7e < str[i]; ++i);
		}

		return out;
	}

	string color_pair::left() const {
		return type == background? get_bg(color) : get_text(color);
	}

	string color_pair::right() const {
		return type == background? reset_bg : reset_fg;
	}

	ansistream::ansistream(): content_out(std::cout), style_out(std::cerr) {}

	ansistream & ansistream::err() {
		static ansistream as(std::cerr, std::cerr);
		return as;
	}

	ansistream & ansistream::left_paren() {
		if (parens_on) {
			*this << dim;
			content_out << "(";
			*this >> dim;
		}

		return *this;
	}

	ansistream & ansistream::right_paren() {
		if (parens_on) {
			parens_on = false;
			*this << dim;
			content_out << ")";
			*this >> dim;
		}

		return *this;
	}

	ansistream & ansistream::flush() {
		content_out.flush();
		style_out.flush();
		return *this;
	}

	ansistream & ansistream::clear() {
		style_out << "\e[2J";
		return *this;
	}

	ansistream & ansistream::jump(int x, int y) {
		if (0 <= x && 0 <= y) {
			style_out << "\e[" << (y + 1) << ";" << (x + 1) << "H";
		} else if (0 <= x) {
			style_out << "\e[" << (x + 1) << "G";
		} else if (0 <= y) {
			style_out << "\e[999999A";
			if (0 < y)
				style_out << "\e[" + std::to_string(y) + "B";
		} else {
			throw std::runtime_error("Invalid jump: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
		}

		return *this;
	}

	ansistream & ansistream::jump() {
		jump(0, 0);
		return *this;
	}

	ansistream & ansistream::save() {
		style_out << "\e[s";
		return *this;
	}

	ansistream & ansistream::restore() {
		style_out << "\e[u";
		return *this;
	}

	ansistream & ansistream::clear_line() {
		style_out << "\e[2K";
		return *this;
	}

	ansistream & ansistream::clear_left() {
		style_out << "\e[1K";
		return *this;
	}

	ansistream & ansistream::clear_right() {
		style_out << "\e[K";
		return *this;
	}

	ansistream & ansistream::up(size_t rows) {
		if (rows != 0)
			style_out << "\e[" + std::to_string(rows) + "A";
		return *this;
	}

	ansistream & ansistream::down(size_t rows) {
		if (rows != 0)
			style_out << "\e[" + std::to_string(rows) + "B";
		return *this;
	}

	ansistream & ansistream::right(size_t cols) {
		if (cols != 0)
			style_out << "\e[" + std::to_string(cols) + "C";
		return *this;
	}

	ansistream & ansistream::left(size_t cols) {
		if (cols != 0)
			style_out << "\e[" + std::to_string(cols) + "D";
		return *this;
	}

	ansistream & ansistream::vpos(size_t y) {
		up(999999);
		if (y > 0)
			down(y);
		return *this;
	}

	ansistream & ansistream::hpos(size_t x) {
		style_out << "\e[" + std::to_string(x + 1) + "H";
		return *this;
	}

	ansistream & ansistream::scroll_up(size_t lines) {
		style_out << "\e[" + std::to_string(lines) + "S";
		return *this;
	}

	ansistream & ansistream::scroll_down(size_t lines) {
		style_out << "\e[" + std::to_string(lines) + "T";
		return *this;
	}

	ansistream & ansistream::delete_chars(size_t count) { // DCH
		style_out << "\e[" + std::to_string(count) + "P";
		return *this;
	}

	ansistream & ansistream::set_origin() {
		style_out << "\e[?6h";
		return *this;
	}

	ansistream & ansistream::reset_origin() {
		style_out << "\e[?6l";
		return *this;
	}

	ansistream & ansistream::hmargins(size_t left, size_t right) {
		style_out << "\e[" + std::to_string(left + 1) + ";" + std::to_string(right + 1) + "s";
		return *this;
	}

	ansistream & ansistream::hmargins() {
		style_out << "\e[s";
		return *this;
	}

	ansistream & ansistream::enable_hmargins() {
		style_out << "\e[?69h";
		return *this;
	}

	ansistream & ansistream::disable_hmargins() {
		style_out << "\e[?69l";
		return *this;
	}

	ansistream & ansistream::vmargins(size_t top, size_t bottom) {
		style_out << "\e[" + std::to_string(top + 1) + ";" + std::to_string(bottom + 1) + "r";
		return *this;
	}

	ansistream & ansistream::vmargins() {
		style_out << "\e[r";
		return *this;
	}

	ansistream & ansistream::margins(size_t top, size_t bottom, size_t left, size_t right) {
		vmargins(top, bottom);
		hmargins(left, right);
		return *this;
	}

	ansistream & ansistream::margins() {
		hmargins();
		vmargins();
		return *this;
	}


	ansistream & ansistream::operator<<(const ansi::color &c) {
		// Adds a text color: "as << red"
		text_color = c;
		style_out << get_text(c);
		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::color_pair &p) {
		// Adds a color, either text or background:
		// - "as << fg(red)"
		// - "as << bg(red)"

		if (p.type == background)
			style_out << get_bg(bg_color = p.color);
		else
			style_out << get_text(text_color = p.color);

		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::style &style) {
		// Adds a style: "as << bold"
		styles.insert(style);
		style_out << style_codes.at(style);
		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::ansi_pair<ansi::style> &pair) {
		// Removes a style: "as << remove(bold)"
		if (pair.add)
			return *this << pair.value;
		return *this >> pair.value;
	}

	ansistream & ansistream::operator<<(const ansi::action &action) {
		// Performs an action on the stream: "as << reset"
		switch (action) {
			case end_line:      *this << "\e[0m" << std::endl; break;
			case reset:         *this << reset_all; break;
			case check:         *this << "["_d << wrap(str_check, ansi::green)  << "] "_d; break;
			case nope:          *this << "["_d << wrap(str_nope,  ansi::red)    << "] "_d; break;
			case warning:       *this << "["_d << wrap("~",       ansi::yellow) << "] "_d; break;
			case information:   *this << "["_d << wrap("i",       ansi::blue)   << "] "_d; break;
			case open_paren:    *this << wrap("(", dim); break;
			case close_paren:   *this << wrap(")", dim); break;
			case enable_parens: parens_on = true; break;
			default:
				throw std::invalid_argument("Invalid action: " + std::to_string(action));
		}

		this->content_out.flush();
		this->style_out.flush();
		return *this;
	}

	// These next three are for manipulator support.

	ansistream & ansistream::operator<<(std::ostream & (*fn)(std::ostream &)) {
		fn(content_out);
		return *this;
	}

	ansistream & ansistream::operator<<(std::ostream & (*fn)(std::ios &)) {
		fn(content_out);
		return *this;
	}

	ansistream & ansistream::operator<<(std::ostream & (*fn)(std::ios_base &)) {
		fn(content_out);
		return *this;
	}

	ansistream & ansistream::operator<<(const char *t) {
		left_paren();
		content_out << t;
		right_paren();
		return *this;
	}

	ansistream & ansistream::operator>>(const ansi::style &style) {
		// Removes a style: "as >> bold"
		styles.erase(style);
		style_out << style_resets.at(style);
		return *this;
	}
}

std::string operator"" _b(const char *str, unsigned long) { return ansi::wrap(str, ansi::bold); }
std::string operator"" _d(const char *str, unsigned long) { return ansi::wrap(str, ansi::dim); }
std::string operator"" _i(const char *str, unsigned long) { return ansi::wrap(str, ansi::italic); }
std::string operator"" _u(const char *str, unsigned long) { return ansi::wrap(str, ansi::underline); }
std::string operator"" _bd(const char *str, unsigned long) { return ansi::wrap(ansi::wrap(str, ansi::bold), ansi::dim); }
