# Лабораторна робота №5

## Дисципліна
**Об'єктно-орієнтоване програмування**

## Тема
**Розробка багатовіконного інтерфейсу користувача для графічного редактора об'єктів**

## Студент
**Чакун Олександр Сергійович**  
Група: **ІМ-о51**  
Номер у списку / варіант: **16**

## Варіант
Оскільки номер **16** парний, `MyEditor` реалізовано як **класичний Singleton**:
- статичний вказівник `p_instance_`;
- закритий конструктор;
- `GetInstance()` створює єдиний об'єкт через `new`;
- `DestroyInstance()` коректно звільняє пам'ять.

## Що реалізовано
- 6 фігур із Lab4;
- `MyEditor` як класичний Singleton;
- динамічний масив `Shape**`;
- незалежний модуль `MyTable`;
- немодальне вікно таблиці через `CreateDialogParamW`;
- 5 полів таблиці: назва, x1, y1, x2, y2;
- вертикальний та горизонтальний скролінг;
- автоматичне додавання кожної нової фігури у таблицю;
- автоматичний запис кожної нової фігури у `objects.txt`;
- `File -> Зберегти об'єкти` переписує поточний набір фігур у `objects.txt`;
- `View -> Таблиця об'єктів` відкриває/закриває немодальне вікно;
- таблиця автоматично закривається при завершенні програми;
- `IsDialogMessageW` використовується для повідомлень немодального діалогу;
- Toolbar на 6 кнопок із підказками збережений із Lab4.

## Структура
```text
Lab5/
├── Lab5.sln
├── Lab5.vcxproj
├── Lab5.vcxproj.filters
├── README.md
├── src/
│   ├── Lab5.cpp
│   ├── resource.h
│   ├── shape.h / shape.cpp
│   ├── point_shape.h / point_shape.cpp
│   ├── line_shape.h / line_shape.cpp
│   ├── rect_shape.h / rect_shape.cpp
│   ├── ellipse_shape.h / ellipse_shape.cpp
│   ├── line_oo_shape.h / line_oo_shape.cpp
│   ├── cube_shape.h / cube_shape.cpp
│   ├── my_editor.h / my_editor.cpp
│   ├── my_table.h / my_table.cpp
│   └── my_table_resources.h
├── res/
│   ├── Lab5.rc
│   ├── my_table.rc
│   └── toolbar.bmp
├── screenshots/
└── report/
    └── assets/
```
