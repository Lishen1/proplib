#include <editor/yaml_highlighter.h>

Yaml_highlighter::Yaml_highlighter(QTextDocument *parent /*= 0*/) : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;

  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList keywordPatterns;
  keywordPatterns << "\\bchar\\b"
                  << "\\bclass\\b"
                  << "\\bconst\\b"
                  << "\\bdouble\\b"
                  << "\\benum\\b"
                  << "\\bexplicit\\b"
                  << "\\bfriend\\b"
                  << "\\binline\\b"
                  << "\\bint\\b"
                  << "\\blong\\b"
                  << "\\bnamespace\\b"
                  << "\\boperator\\b"
                  << "\\bprivate\\b"
                  << "\\bprotected\\b"
                  << "\\bpublic\\b"
                  << "\\bshort\\b"
                  << "\\bsignals\\b"
                  << "\\bsigned\\b"
                  << "\\bslots\\b"
                  << "\\bstatic\\b"
                  << "\\bstruct\\b"
                  << "\\btemplate\\b"
                  << "\\btypedef\\b"
                  << "\\btypename\\b"
                  << "\\bunion\\b"
                  << "\\bunsigned\\b"
                  << "\\bvirtual\\b"
                  << "\\bvoid\\b"
                  << "\\bvolatile\\b"
                  << "\\bbool\\b";
  foreach (const QString &pattern, keywordPatterns)
  {
    rule.pattern = QRegularExpression(pattern);
    rule.format  = keywordFormat;
    highlightingRules.append(rule);
  }

  classFormat.setFontWeight(QFont::Bold);
  classFormat.setForeground(Qt::darkMagenta);
  rule.pattern = QRegularExpression(R"(\|)");
  rule.format  = classFormat;
  highlightingRules.append(rule);

  quotationFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegularExpression(R"(\\[bBAZzG]|\^|\$)");
  rule.format  = quotationFormat;
  highlightingRules.append(rule);

  functionFormat.setFontItalic(true);
  functionFormat.setForeground(Qt::blue);
  rule.pattern = QRegularExpression(R"(\\b[A-Za-z0-9_]+(?=\\())");
  rule.format  = functionFormat;
  highlightingRules.append(rule);

  singleLineCommentFormat.setForeground(Qt::red);
  rule.pattern = QRegularExpression(R"(//[^\n]*)");
  rule.format  = singleLineCommentFormat;
  highlightingRules.append(rule);

  numberFormat.setForeground(Qt::red);
  rule.pattern = QRegularExpression(R"(\W[0-9]{1,9}(\.{0,}[0-9]{0,}))");
  rule.format = numberFormat;
  highlightingRules.append(rule);

  multiLineCommentFormat.setForeground(Qt::red);

  commentStartExpression = QRegularExpression("/\\*");
  commentEndExpression   = QRegularExpression("\\*/");
}

void Yaml_highlighter::highlightBlock(const QString &text)
{
  foreach(const HighlightingRule &rule, highlightingRules)
  {
    QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
    while (matchIterator.hasNext())
    {
      QRegularExpressionMatch match = matchIterator.next();
      setFormat(match.capturedStart(), match.capturedLength(), rule.format);
    }
  }
}
