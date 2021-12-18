declare i32 @atoi(i8*)
declare i32 @printf(i8*, ...)

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

define i32 @factorial(i32) {
	%n = alloca i32
	store i32 %0, i32* %n

	%2 = load i32, i32* %n
	%3 = alloca i32
	store i32 0, i32* %3
	%4 = load i32, i32* %3
	%5 = icmp eq i32 %2, %4
	%6 = icmp eq i1 %5, 0
	br i1 %6, label %100, label %101

;	<label>:100:		; preds = %1
	%8 = alloca i32
	store i32 1, i32* %8
	%9 = load i32, i32* %8

	br label %102

;	<label>:101:		; preds = %1
	%11 = load i32, i32* %n
	%12 = load i32, i32* %n
	%13 = alloca i32
	store i32 1, i32* %13
	%14 = load i32, i32* %13
	%15 = sub nsw i32 %12, %14
	%16 = call i32 @factorial(i32 %15)
	%17 = mul nsw i32 %11, %15
	br label %102

;	<label>:102:		; preds = false, true
	%19 = load i32, i32* %8
	ret i32 %19

}

define i32 @main(i32, i8**) {
	%argc = alloca i32
	store i32 %0, i32* %argc
	%argv = alloca i8**
	store i8** %1, i8*** %argv

	%argument = alloca i32

	%2 = load i8**, i8*** %argv
	%3 = getelementptr inbounds i8*, i8** %2, i64 1
	%4 = load i8*, i8** %3
	%5 = call i32 @atoi(i8* %4)
	store i32 %6, i32* %argument

	%7 = load i32, i32* %argument
	%8 = call i32 @factorial(i32 %7)
	%9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %8)

	ret i32 0
}
