# Formal BNF Grammar for Experimental WAS Syntax [![Build Status](https://travis-ci.org/mbebenita/was.svg?branch=master)](https://travis-ci.org/mbebenita/was)

Flex/Bison parser for `.was` files.

To build and run regression tests use:
```
make
make test
```

To parse a single file and print its grammar production rules, use:

```
./was test/test.was --debug
```

## [Sample Was File](test/perlin.pass)

# Lexical Structure

Was input is interpreted as a sequence of Unicode code points encoded in UTF-8.
Most Was grammar rules are defined in terms of printable ASCII-range code points, but a small number are defined in terms of Unicode properties or explicit code point lists.

## Identifiers

An identifier is prefixed with `$` characters. The identifier can be any stream of bytes including with zero length:

 - The characters must be one of ASCII chars: `[0-9a-zA-Z_$]`;
 - Or, hex encoded byte in form: `\` HEXDIGIT HEXDIGIT, where HEXDIGIT is `[0-9a-fA-F]`.
 
Examples of valid identifiers include: `$abc`, `$0`, `$_0`, `$\00` and '$'.
The `$` sigil prefix on identifiers cleanly ensures that they never collide with as keywords.

## Comments

Comments in Was code follow the general C++ style of line (`//`) and block (`/* ... */`) comment forms. Nested block comments are supported.

```
// This is a comment
/* This is also a comment */
/* This is a comment
/* as well */

```
## Whitespace

Whitespace is any non-empty string containing only the following characters:

- `U+0020` (space, ` `)
- `U+0009` (tab, `\t`)
- `U+000B` (vertical tab, `\v`)
- `U+000A` (line feed, `\n`)
- `U+000C` (form feed, `\f`)
- `U+000D` (carriage return, `\r`)
 
Was is a "free-form" language, meaning that all forms of whitespace serve only to separate tokens in the grammar, and have no semantic significance.
A Was program has identical meaning if each whitespace element is replaced with any other legal whitespace element, such as a single space character.

## Semicolons

Was does not require you to write a semicolon `;` after a statement or expression.
In fact, Was does not have statements, it only has expressions which are space delimited.

## Literals

### Number literals

| Number literals     | Example                                        | Suffixes                |
| ------------------- |:---------------------------------------------- |:----------------------- |
| Decimal integer     | 123                                            | Integer suffixes        |
| Hex integer         | 0xff                                           | Integer suffixes        |
| Octal integer       | 0o77                                           | Integer suffixes        |
| Binary integer      | 0b11001100                                     | Integer suffixes        |
| Floating-point      | 123.0E+77                                      | Floating-point suffixes |


Number literals may be followed (immediately, without any spaces) by an integer or floating-point suffix, which forcibly sets the type of the literal.
The integer suffix must be the name of one of the integral types: `u32`, `i32`, `u64`, `i64`.
The floating-point suffix must be the name of one of the floating-point types: `f32` or `f64`.


## Control flow operators ([described here](https://github.com/WebAssembly/design/blob/master/AstSemantics.md))

| Name | Syntax | Examples
| ---- | ---- | ---- |
| `nop` | `nop`
| `block` | `{` … [ *break-label* `:` ] `}` | `{}`, `{ br $a nop $a:}`
| `loop` | `loop` [ *continue-label* ] `{` …  [ *break-label* `:` ] `}` | `loop $a { br $a }`
| `if` | `if` `(` *expr* `)` `{` *expr* * `}` | `if 0 { 1 }`
| `if_else` | `if` `(` *expr* `)` `{` *expr* * `}` `else` `{` *expr* *`}` | `if 0 { 1 } else { 2 }`
| `select` | `select` *expr* `,` *expr* `?` *expr* | `select 1, 2 ? $x < $y`
| `br` | `br` [ `(` *expr* `)` ] *label* | `br $a`
| `br_if` | `br_if` `(` *expr [ ',' *expr* ] `)` *label* | `br_if $x < $y, 1, $a`
| `br_table` | `br_table` `(` *expr* [ ',' *expr* ] `)` `[` *case-label* `,` … `]` *default-label*  | `br_table 1, [$x, $y], $z`
| `return` | `return` [ *expr* ] | `return`
| `unreachable` | `unreachable` | `unreachable`

## Basic operators ([described here](https://github.com/WebAssembly/design/blob/master/AstSemantics.md#constants))

| Name | Syntax | Example
| ---- | ---- | ---- |
| `i32.const` | …`i32`<sub>opt</sub> | `123`, `123i32`, `0b101`
| `i64.const` | …`i64` | `456i64`
| `f64.const` | …`f32` or …`f` | `0.1f`
| `f32.const` | …`f64`<sub>opt</sub> | `0.2f64`
| `get_local` | *name* | `$x`
| `set_local` | *name* `=` *expr* | `$x = $x + 1`
| `call` | `call` *name* `(`*expr* `,` … `)` | `call $min(0, 2)`
| `call_import` | `call_import` *name* `(`*expr* `,` … `)` | `call_import $max(0, 2)`
| `call_indirect` | `call_indirect` *signature-name* `[` *expr* `]` `(`*expr* `,` … `)` | `call_indirect $type$1 [$i] $min(0, 2)`

## Memory-related operators ([described here](https://github.com/WebAssembly/design/blob/master/AstSemantics.md#linear-memory-accesses))

| Name | Syntax | Example
| ---- | ---- | ---- |
| *memory-immediate* | `[` *base-expression* `,` *offset* `]` | `[1 + 2, +4]`
| `i32.load8_s` | `i32.load8_s` *memory-immediate* | `i32.load8_s [0, +4]` 
| `i32.load8_u` | … | … 
| `i32.load16_s` | … | … 
| `i32.load16_u` | … | … 
| `i64.load8_s` | … | … 
| `i64.load8_u` | … | … 
| `i64.load16_s` | … | … 
| `i64.load16_u` | … | … 
| `i64.load32_s` | … | … 
| `i64.load32_u` | … | … 
| `i32.load` | … | … 
| `i64.load` | … | … 
| `f32.load` | … | … 
| `f64.load` | … | … 
| `i32.store8` | `i32.store8` *memory-immediate* `,` *expr* | `i32.store8 [0, +4], 1 + 2` 
| `i32.store16` | … | … 
| `i64.store8` | … | … 
| `i64.store16` | … | … 
| `i64.store32` | … | … 
| `i32.store` | … | … 
| `i64.store` | … | … 
| `f32.store` | … | … 
| `f64.store` | … | … 
| `memory_size`
| `grow_memory`

## Simple operators ([described here](AstSemantics#32-bit-integer-operators))

| Name | Syntax |
| ---- | ---- |
| `i32.add` | … `+` …
| `i32.sub` | … `-` …
| `i32.mul` | … `*` …
| `i32.div_s` | … `/s` …
| `i32.div_u` | … `/u` …
| `i32.rem_s` | … `%s` …
| `i32.rem_u` | … `%u` …
| `i32.and` | … `&` …
| `i32.or` | … `\|` …
| `i32.xor` | … `^` …
| `i32.shl` | … `<<` …
| `i32.shr_u` | … `>>` …
| `i32.shr_s` | … `>>>` …
| `i32.eq` | … `==` …
| `i32.ne` | … `!=` …
| `i32.lt_s` | … `<s` … 
| `i32.le_s` | … `<=s` …
| `i32.lt_u` | … `<u` …
| `i32.le_u` | … `<=u` …
| `i32.gt_s` | … `>s` …
| `i32.ge_s` | … `>=s` …
| `i32.gt_u` | … `>u` …
| `i32.ge_u` | … `>=u` …
| `i32.clz` | `i32.clz` …
| `i32.ctz` | `i32.ctz` …
| `i32.eqz` | `i32.eqz` …
| `i32.popcnt` | `i32.popcnt` …
| `i64.add` | … `+` …
| `i64.sub` | … `-` …
| `i64.mul` | … `*` …
| `i64.div_s` | … `/s` …
| `i64.div_u` | … `/u` …
| `i64.rem_s` | … `%s` …
| `i64.rem_u` | … `%u` …
| `i64.and` | … `&` …
| `i64.or` | … `\|` …
| `i64.xor` | … `^` …
| `i64.shl` | … `<<` …
| `i64.shr_u` | … `>>` …
| `i64.shr_s` | … `>>>` …
| `i64.eq` | … `==` …
| `i64.ne` | … `!=` …
| `i64.lt_s` | … `<s` … 
| `i64.le_s` | … `<=s` …
| `i64.lt_u` | … `<u` …
| `i64.le_u` | … `<=u` …
| `i64.gt_s` | … `>s` …
| `i64.ge_s` | … `>=s` …
| `i64.gt_u` | … `>u` …
| `i64.ge_u` | … `>=u` …
| `i64.clz` | `i64.clz` …
| `i64.ctz` | `i64.ctz` …
| `i64.eqz` | `i64.eqz` …
| `i64.popcnt` | `i32.popcnt` …
| `f32.add` | … `+` …
| `f32.sub` | … `-` …
| `f32.mul` | … `*` …
| `f32.div` | … `/` …
| `f32.min` | `f32.min` …
| `f32.max` | `f32.max` …
| `f32.abs` | `f32.abs` …
| `f32.neg` | `f32.neg` …
| `f32.copysign` | `f32.copysign` …
| `f32.ceil` | `f32.ceil` …
| `f32.floor` | `f32.floor` …
| `f32.trunc` | `f32.trunc` …
| `f32.nearest` | `f32.nearest` …
| `f32.sqrt` | `f32.sqrt`
| `f32.eq` | … `==` …
| `f32.ne` | … `!=` …
| `f32.lt` | … `<` …
| `f32.le` | … `<=` …
| `f32.gt` | … `>` …
| `f32.ge` | … `>=` …
| `f64.add` | … `+` …
| `f64.sub` | … `-` …
| `f64.mul` | … `*` …
| `f64.div` | … `/` …
| `f64.min` | `f64.min` …
| `f64.max` | `f64.max` …
| `f64.abs` | `f64.abs` …
| `f64.neg` | `f64.neg` …
| `f64.copysign` | `f64.copysign` …
| `f64.ceil` | `f64.ceil` …
| `f64.floor` | `f64.floor` …
| `f64.trunc` | `f64.trunc` …
| `f64.nearest` | `f64.nearest` …
| `f64.sqrt` | `f64.sqrt` …
| `f64.eq` | … `==` … 
| `f64.ne` | … `!=` …
| `f64.lt` | … `<` …
| `f64.le` | … `<=` …
| `f64.gt` | … `>` …
| `f64.ge` | … `>=` …

| Name | Syntax |
| ---- | ---- |
| `i32.trunc_s/f32` | `i32.trunc_s/f32`
| `i32.trunc_s/f64` | …
| `i32.trunc_u/f32` | …
| `i32.trunc_u/f64` | …
| `i32.wrap/i64` | …
| `i64.trunc_s/f32` | …
| `i64.trunc_s/f64` | …
| `i64.trunc_u/f32` | …
| `i64.trunc_u/f64` | …
| `i64.extend_s/i32` | …
| `i64.extend_u/i32` | …
| `f32.convert_s/i32` | …
| `f32.convert_u/i32` | …
| `f32.convert_s/i64` | …
| `f32.convert_u/i64` | …
| `f32.demote/f64` | …
| `f32.reinterpret/i32` | …
| `f64.convert_s/i32` | …
| `f64.convert_u/i32` | …
| `f64.convert_s/i64` | …
| `f64.convert_u/i64` | …
| `f64.promote/f32` | …
| `f64.reinterpret/i64` | …
| `i32.reinterpret/f32` | …
| `i64.reinterpret/f64` | …

| Name | Precedence | Direction | Example(s) |
| ---- | ---- | ---- | ---- |
| statements | 16 | | `if (…) { … }`, ` { … }` |
| literal | 15 | | `2.0` |
| variable | 15 | | `$a` |
| group | 15 | | `( … )` |
| call | 15 | | `call`, `call_import` |
| load | 15 | | `i32.load` |
| operators | 15 | | `f64.floor`, `f32.convert_s/i32`, `f32.min`, `select(…, …, …)` |
| negate | 12 | | `!` |
| mutplication | 11 | left | `*`, `/` |
| addition | 10 | left | `+`, `-` |
| bitwise shift | 9 | left | `<<` |
| comparison | 8 | left | `<`, `>=` |
| equality | 7 | left | `==`, `!=` |
| bitwise and | 6 | left | `&` |
| bitwise xor | 5 | left | `^` |
| bitwise or | 4 | left | `|` |
| store | 2 | | `i32.store` |
| assignment | 1 | right | `$a = …` |
