declare i32 @atoi(i8*)
declare i32 @printf(i8*, ...)

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

define i32 @factorial(i32) {
	%n = alloca i32
	store i32 %0, i32* %n

	%return = alloca i32
	%2 = load i32, i32* %n
	%3 = alloca i32
	store i32 0, i32* %3
	%4 = load i32, i32* %3
	%5 = icmp eq i32 %2, %4
	%6 = icmp eq i1 %5, 1
	br i1 %6, label %btrue, label %bfalse

btrue:
	%7 = alloca i32
	store i32 1, i32* %7
	%8 = load i32, i32* %7
	store i32 %8, i32* %return
	br label %end

bfalse:
	%9 = load i32, i32* %n
	%10 = load i32, i32* %n
	%11 = alloca i32
	store i32 1, i32* %11
	%12 = load i32, i32* %11
	%13 = sub nsw i32 %10, %12
	%14 = call i32 @factorial(i32 %13)
	%15 = mul nsw i32 %9, %14
	store i32 %15, i32* %return
	br label %end

end:			; preds = bfalse, btrue
	%16 = load i32, i32* %return
	ret i32 %16

}

define i32 @main(i32, i8**) {
	%argc = alloca i32
	store i32 %0, i32* %argc
	%argv = alloca i8**
	store i8** %1, i8*** %argv

	%argument = alloca i32

	%return = alloca i32
	%3 = load i8**, i8*** %argv
	%4 = getelementptr inbounds i8*, i8** %3, i64 1
	%5 = load i8*, i8** %4
	%6 = call i32 @atoi(i8* %5)
	store i32 %6, i32* %argument

	%7 = load i32, i32* %argument
	%8 = call i32 @factorial(i32 %7)
	%9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %8)

	ret i32 0
}
