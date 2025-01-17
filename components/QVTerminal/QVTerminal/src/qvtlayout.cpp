#include "qvtlayout.h"

QVTLayout::QVTLayout()
{
    appendLine();
}

int QVTLayout::lineCount() const
{
    return _lines.count();
}

QVTLine &QVTLayout::lineAt(int i)
{
    return _lines[i];
}

void QVTLayout::clear()
{
    _lines.clear();
    appendLine();
}

void QVTLayout::appendLine()
{
    _lines.append(QVTLine());
}
