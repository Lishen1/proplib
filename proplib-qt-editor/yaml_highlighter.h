#pragma once
#include <QSyntaxHighlighter>
#include <QRegularExpression>

class Yaml_highlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  Yaml_highlighter(QTextDocument *parent = 0);

protected:
  void highlightBlock(const QString &text) override;

private:
  struct HighlightingRule
  {
    QRegularExpression pattern;
    QTextCharFormat format;
  };
  QVector<HighlightingRule> highlightingRules;

  QRegularExpression commentStartExpression;
  QRegularExpression commentEndExpression;

  QTextCharFormat keywordFormat;
  QTextCharFormat classFormat;
  QTextCharFormat singleLineCommentFormat;
  QTextCharFormat multiLineCommentFormat;
  QTextCharFormat quotationFormat;
  QTextCharFormat functionFormat;
};
