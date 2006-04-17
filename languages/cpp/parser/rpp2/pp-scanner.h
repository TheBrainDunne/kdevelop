/*
  Copyright 2005 Roberto Raggi <roberto@kdevelop.org>
  Copyright 2006 Hamish Rodda <rodda@kde.org>

  Permission to use, copy, modify, distribute, and sell this software and its
  documentation for any purpose is hereby granted without fee, provided that
  the above copyright notice appear in all copies and that both that
  copyright notice and this permission notice appear in supporting
  documentation.

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  KDEVELOP TEAM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
  AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef PP_SCANNER_H
#define PP_SCANNER_H

#include <QtGlobal>

class pp_skip
{
public:
  virtual void operator()(QTextStream& input, QTextStream& output) = 0;

  int linesSkipped() const;

  /// Move back \a offset chars in the stream
  void rewind(QTextStream& stream, qint64 offset = 1) const;

protected:
  int m_lines;
}

class pp_skip_blanks : public pp_skip
{
public:
  virtual void operator()(QTextStream& input, QTextStream& output);
};

class pp_skip_comment_or_divop : public pp_skip
{
public:
  virtual void operator()(QTextStream& input, QTextStream& output);
};

class pp_skip_identifier : public pp_skip
{
public:
  virtual void operator()(QTextStream& input, QTextStream& output);
};

class pp_skip_number : public pp_skip
{
public:
  virtual void operator()(QTextStream& input, QTextStream& output);
};

class pp_skip_string_literal : public pp_skip
{
public:
  virtual void operator()(QTextStream& input, QTextStream& output);
};

class pp_skip_char_literal : public pp_skip
{
public:
  virtual void operator()(QTextStream& input, QTextStream& output);
};

class pp_skip_argument : public pp_skip
{
public:
  virtual void operator()(QTextStream& input, QTextStream& output);

private:
  pp_skip_identifier skip_number;
  pp_skip_identifier skip_identifier;
  pp_skip_string_literal skip_string_literal;
  pp_skip_char_literal skip_char_literal;
  pp_skip_comment_or_divop skip_comment_or_divop;
};

#endif // PP_SCANNER_H

// kate: indent-width 2;
