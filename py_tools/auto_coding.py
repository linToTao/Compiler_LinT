# 由于 C++ 不支持反射，因此只能通过自动生成代码的方式实现从 LexType 和 NodeType 的映射

lex_file = '../TestCase/SNL_Core/lex_type_list.txt'
lex_str_map = 'mLex2String'

node_file = '../TestCase/SNL_Core/node_type_list.txt'
node_str_map = 'mNodeType2Str'

with open(lex_file, 'r') as f:
    for lex in f:
        if lex[0: 2] == '//':
            continue
        lex = lex[0: -1] if lex[-1] == '\n' else lex
        print(lex_str_map + '[LexType::' + lex + '] = _T("<' + lex + '>");', end='\n')

print()

with open(node_file, 'r') as f:
    for node in f:
        if node[0: 2] == '//':
            continue
        node = node[0: -1] if node[-1] == '\n' else node
        print(node_str_map + '[NodeType::' + node + '] = _T("' + node + '");', end='\n')
