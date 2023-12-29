#[LLVM IR]:
; ModuleID = 'top'
source_filename = "top"

@GEN_SIZE = external global i32
@BLOCK_SIZE = external global i32
@WINDOW_WIDTH = external global i32
@REPEAT_SIZE = external global i32
@genf = external global [2500 x i32]
@gens = external global [2500 x i32]

declare void @simPutPixel(i32, i32, i32)

declare void @simFlush(void)

declare i32 @simRand(void)

define void @draw_shape(i32 %0, i32 %1, i32 %2) {
"0":
  %3 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  %4 = alloca i32, align 4
  store i32 %1, i32* %4, align 4
  %5 = alloca i32, align 4
  store i32 %2, i32* %5, align 4
  %6 = alloca i32, align 4
  store i32 0, i32* %6, align 4
  %7 = alloca i32, align 4
  store i32 0, i32* %7, align 4
  %8 = load i32, i32* %3, align 4
  %9 = load i32, i32* @BLOCK_SIZE, align 4
  %10 = mul i32 %8, %9
  %11 = load i32, i32* %3, align 4
  %12 = add i32 %11, 1
  %13 = load i32, i32* @BLOCK_SIZE, align 4
  %14 = mul i32 %12, %13
  store i32 %10, i32* %6, align 4
  br label %"1"

"1":                                              ; preds = %"6", %"0"
  %15 = load i32, i32* %6, align 4
  %16 = icmp slt i32 %15, %14
  br i1 %16, label %"2", label %"3"

"2":                                              ; preds = %"1"
  %17 = load i32, i32* %4, align 4
  %18 = load i32, i32* @BLOCK_SIZE, align 4
  %19 = mul i32 %17, %18
  %20 = load i32, i32* %4, align 4
  %21 = add i32 %20, 1
  %22 = load i32, i32* @BLOCK_SIZE, align 4
  %23 = mul i32 %21, %22
  store i32 %19, i32* %7, align 4
  br label %"4"

"3":                                              ; preds = %"1"
  ret void

"4":                                              ; preds = %"5", %"2"
  %24 = load i32, i32* %7, align 4
  %25 = icmp slt i32 %24, %23
  br i1 %25, label %"5", label %"6"

"5":                                              ; preds = %"4"
  %26 = load i32, i32* %6, align 4
  %27 = load i32, i32* %7, align 4
  %28 = load i32, i32* %5, align 4
  call void @simPutPixel(i32 %26, i32 %27, i32 %28)
  %29 = load i32, i32* %7, align 4
  %30 = add i32 %29, 1
  store i32 %30, i32* %7, align 4
  br label %"4"

"6":                                              ; preds = %"4"
  %31 = load i32, i32* %6, align 4
  %32 = add i32 %31, 1
  store i32 %32, i32* %6, align 4
  br label %"1"
}

define void @drawGen() {
"7":
  %0 = alloca i32, align 4
  store i32 0, i32* %0, align 4
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 0, i32* %0, align 4
  %2 = load i32, i32* @GEN_SIZE, align 4
  store i32 0, i32* %1, align 4
  br label %"8"

"8":                                              ; preds = %"13", %"7"
  %3 = load i32, i32* %1, align 4
  %4 = icmp slt i32 %3, %2
  br i1 %4, label %"9", label %"10"

"9":                                              ; preds = %"8"
  %5 = load i32, i32* @GEN_SIZE, align 4
  store i32 0, i32* %0, align 4
  br label %"11"

"10":                                             ; preds = %"8"
  ret void

"11":                                             ; preds = %"12", %"9"
  %6 = load i32, i32* %0, align 4
  %7 = icmp slt i32 %6, %5
  br i1 %7, label %"12", label %"13"

"12":                                             ; preds = %"11"
  %8 = load i32, i32* %0, align 4
  %9 = load i32, i32* %1, align 4
  %10 = load i32, i32* %1, align 4
  %11 = mul i32 %10, 50
  %12 = load i32, i32* %0, align 4
  %13 = add i32 %11, %12
  %14 = getelementptr [2500 x i32], [2500 x i32]* @genf, i32 0, i32 %13
  %15 = load i32, i32* %14, align 4
  %16 = mul i32 65280, %15
  %17 = add i32 -16777216, %16
  call void @draw_shape(i32 %8, i32 %9, i32 %17)
  %18 = load i32, i32* %0, align 4
  %19 = add i32 %18, 1
  store i32 %19, i32* %0, align 4
  br label %"11"

"13":                                             ; preds = %"11"
  %20 = load i32, i32* %1, align 4
  %21 = add i32 %20, 1
  store i32 %21, i32* %1, align 4
  br label %"8"
}

define void @calcGen() {
"14":
  %0 = alloca i32, align 4
  store i32 0, i32* %0, align 4
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %2, align 4
  %3 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  %4 = alloca i32, align 4
  store i32 0, i32* %4, align 4
  %5 = load i32, i32* @GEN_SIZE, align 4
  store i32 0, i32* %1, align 4
  br label %"15"

"15":                                             ; preds = %"20", %"14"
  %6 = load i32, i32* %1, align 4
  %7 = icmp slt i32 %6, %5
  br i1 %7, label %"16", label %"17"

"16":                                             ; preds = %"15"
  %8 = load i32, i32* @GEN_SIZE, align 4
  store i32 0, i32* %0, align 4
  br label %"18"

"17":                                             ; preds = %"15"
  ret void

"18":                                             ; preds = %"29", %"16"
  %9 = load i32, i32* %0, align 4
  %10 = icmp slt i32 %9, %8
  br i1 %10, label %"19", label %"20"

"19":                                             ; preds = %"18"
  store i32 0, i32* %4, align 4
  store i32 -1, i32* %2, align 4
  br label %"21"

"20":                                             ; preds = %"18"
  %11 = load i32, i32* %1, align 4
  %12 = add i32 %11, 1
  store i32 %12, i32* %1, align 4
  br label %"15"

"21":                                             ; preds = %"26", %"19"
  %13 = load i32, i32* %2, align 4
  %14 = icmp slt i32 %13, 2
  br i1 %14, label %"22", label %"23"

"22":                                             ; preds = %"21"
  store i32 -1, i32* %3, align 4
  br label %"24"

"23":                                             ; preds = %"21"
  %15 = load i32, i32* %4, align 4
  %16 = load i32, i32* %1, align 4
  %17 = load i32, i32* @GEN_SIZE, align 4
  %18 = mul i32 %16, %17
  %19 = load i32, i32* %0, align 4
  %20 = add i32 %18, %19
  %21 = getelementptr [2500 x i32], [2500 x i32]* @gens, i32 0, i32 %20
  %22 = load i32, i32* %21, align 4
  %23 = sub i32 %15, %22
  store i32 %23, i32* %4, align 4
  %24 = load i32, i32* %1, align 4
  %25 = load i32, i32* @GEN_SIZE, align 4
  %26 = mul i32 %24, %25
  %27 = load i32, i32* %0, align 4
  %28 = add i32 %26, %27
  %29 = getelementptr [2500 x i32], [2500 x i32]* @gens, i32 0, i32 %28
  %30 = load i32, i32* %29, align 4
  %31 = icmp ne i32 %30, 0
  br i1 %31, label %"27", label %"28"

"24":                                             ; preds = %"25", %"22"
  %32 = load i32, i32* %3, align 4
  %33 = icmp slt i32 %32, 2
  br i1 %33, label %"25", label %"26"

"25":                                             ; preds = %"24"
  %34 = load i32, i32* %4, align 4
  %35 = load i32, i32* %1, align 4
  %36 = load i32, i32* %3, align 4
  %37 = add i32 %35, %36
  %38 = load i32, i32* @GEN_SIZE, align 4
  %39 = add i32 %37, %38
  %40 = load i32, i32* @GEN_SIZE, align 4
  %41 = srem i32 %39, %40
  %42 = load i32, i32* @GEN_SIZE, align 4
  %43 = mul i32 %41, %42
  %44 = load i32, i32* %0, align 4
  %45 = load i32, i32* %2, align 4
  %46 = add i32 %44, %45
  %47 = load i32, i32* @GEN_SIZE, align 4
  %48 = add i32 %46, %47
  %49 = load i32, i32* @GEN_SIZE, align 4
  %50 = srem i32 %48, %49
  %51 = add i32 %43, %50
  %52 = getelementptr [2500 x i32], [2500 x i32]* @gens, i32 0, i32 %51
  %53 = load i32, i32* %52, align 4
  %54 = add i32 %34, %53
  store i32 %54, i32* %4, align 4
  %55 = load i32, i32* %3, align 4
  %56 = add i32 %55, 1
  store i32 %56, i32* %3, align 4
  br label %"24"

"26":                                             ; preds = %"24"
  %57 = load i32, i32* %2, align 4
  %58 = add i32 %57, 1
  store i32 %58, i32* %2, align 4
  br label %"21"

"27":                                             ; preds = %"23"
  %59 = load i32, i32* %4, align 4
  %60 = icmp eq i32 %59, 2
  %61 = zext i1 %60 to i32
  %62 = load i32, i32* %4, align 4
  %63 = icmp eq i32 %62, 3
  %64 = zext i1 %63 to i32
  %65 = icmp ne i32 %61, 0
  %66 = zext i1 %65 to i32
  %67 = icmp ne i32 %64, 0
  %68 = zext i1 %67 to i32
  %69 = add i32 %66, %68
  %70 = icmp ne i32 %69, 0
  br i1 %70, label %"30", label %"31"

"28":                                             ; preds = %"23"
  %71 = load i32, i32* %4, align 4
  %72 = icmp eq i32 %71, 3
  %73 = zext i1 %72 to i32
  %74 = icmp ne i32 %73, 0
  br i1 %74, label %"33", label %"34"

"29":                                             ; preds = %"35", %"32"
  %75 = load i32, i32* %0, align 4
  %76 = add i32 %75, 1
  store i32 %76, i32* %0, align 4
  br label %"18"

"30":                                             ; preds = %"27"
  %77 = load i32, i32* %1, align 4
  %78 = load i32, i32* @GEN_SIZE, align 4
  %79 = mul i32 %77, %78
  %80 = load i32, i32* %0, align 4
  %81 = add i32 %79, %80
  %82 = getelementptr [2500 x i32], [2500 x i32]* @genf, i32 0, i32 %81
  store i32 1, i32* %82, align 4
  br label %"32"

"31":                                             ; preds = %"27"
  %83 = load i32, i32* %1, align 4
  %84 = load i32, i32* @GEN_SIZE, align 4
  %85 = mul i32 %83, %84
  %86 = load i32, i32* %0, align 4
  %87 = add i32 %85, %86
  %88 = getelementptr [2500 x i32], [2500 x i32]* @genf, i32 0, i32 %87
  store i32 0, i32* %88, align 4
  br label %"32"

"32":                                             ; preds = %"31", %"30"
  br label %"29"

"33":                                             ; preds = %"28"
  %89 = load i32, i32* %1, align 4
  %90 = load i32, i32* @GEN_SIZE, align 4
  %91 = mul i32 %89, %90
  %92 = load i32, i32* %0, align 4
  %93 = add i32 %91, %92
  %94 = getelementptr [2500 x i32], [2500 x i32]* @genf, i32 0, i32 %93
  store i32 1, i32* %94, align 4
  br label %"35"

"34":                                             ; preds = %"28"
  %95 = load i32, i32* %1, align 4
  %96 = load i32, i32* @GEN_SIZE, align 4
  %97 = mul i32 %95, %96
  %98 = load i32, i32* %0, align 4
  %99 = add i32 %97, %98
  %100 = getelementptr [2500 x i32], [2500 x i32]* @genf, i32 0, i32 %99
  store i32 0, i32* %100, align 4
  br label %"35"

"35":                                             ; preds = %"34", %"33"
  br label %"29"
}

define void @swap() {
"36":
  %0 = alloca i32, align 4
  store i32 0, i32* %0, align 4
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = load i32, i32* @GEN_SIZE, align 4
  %3 = load i32, i32* @GEN_SIZE, align 4
  %4 = mul i32 %2, %3
  store i32 0, i32* %1, align 4
  br label %"37"

"37":                                             ; preds = %"38", %"36"
  %5 = load i32, i32* %1, align 4
  %6 = icmp slt i32 %5, %4
  br i1 %6, label %"38", label %"39"

"38":                                             ; preds = %"37"
  %7 = load i32, i32* %1, align 4
  %8 = getelementptr [2500 x i32], [2500 x i32]* @genf, i32 0, i32 %7
  %9 = load i32, i32* %8, align 4
  store i32 %9, i32* %0, align 4
  %10 = load i32, i32* %1, align 4
  %11 = getelementptr [2500 x i32], [2500 x i32]* @genf, i32 0, i32 %10
  %12 = load i32, i32* %1, align 4
  %13 = getelementptr [2500 x i32], [2500 x i32]* @gens, i32 0, i32 %12
  %14 = load i32, i32* %13, align 4
  store i32 %14, i32* %11, align 4
  %15 = load i32, i32* %1, align 4
  %16 = getelementptr [2500 x i32], [2500 x i32]* @gens, i32 0, i32 %15
  %17 = load i32, i32* %0, align 4
  store i32 %17, i32* %16, align 4
  %18 = load i32, i32* %1, align 4
  %19 = add i32 %18, 1
  store i32 %19, i32* %1, align 4
  br label %"37"

"39":                                             ; preds = %"37"
  ret void
}

define void @main() {
"40":
  %0 = alloca i32, align 4
  store i32 0, i32* %0, align 4
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %2, align 4
  %3 = load i32, i32* @GEN_SIZE, align 4
  store i32 0, i32* %0, align 4
  br label %"41"

"41":                                             ; preds = %"46", %"40"
  %4 = load i32, i32* %0, align 4
  %5 = icmp slt i32 %4, %3
  br i1 %5, label %"42", label %"43"

"42":                                             ; preds = %"41"
  %6 = load i32, i32* @GEN_SIZE, align 4
  store i32 0, i32* %1, align 4
  br label %"44"

"43":                                             ; preds = %"41"
  %7 = load i32, i32* @REPEAT_SIZE, align 4
  store i32 0, i32* %2, align 4
  br label %"47"

"44":                                             ; preds = %"45", %"42"
  %8 = load i32, i32* %1, align 4
  %9 = icmp slt i32 %8, %6
  br i1 %9, label %"45", label %"46"

"45":                                             ; preds = %"44"
  %10 = load i32, i32* %1, align 4
  %11 = load i32, i32* @GEN_SIZE, align 4
  %12 = mul i32 %10, %11
  %13 = load i32, i32* %0, align 4
  %14 = add i32 %12, %13
  %15 = getelementptr [2500 x i32], [2500 x i32]* @gens, i32 0, i32 %14
  %16 = call i32 @simRand()
  %17 = srem i32 %16, 6
  %18 = icmp eq i32 %17, 0
  %19 = zext i1 %18 to i32
  store i32 %19, i32* %15, align 4
  %20 = load i32, i32* %1, align 4
  %21 = add i32 %20, 1
  store i32 %21, i32* %1, align 4
  br label %"44"

"46":                                             ; preds = %"44"
  %22 = load i32, i32* %0, align 4
  %23 = add i32 %22, 1
  store i32 %23, i32* %0, align 4
  br label %"41"

"47":                                             ; preds = %"48", %"43"
  %24 = load i32, i32* %2, align 4
  %25 = icmp slt i32 %24, %7
  br i1 %25, label %"48", label %"49"

"48":                                             ; preds = %"47"
  call void @calcGen()
  call void @drawGen()
  call void @swap()
  call void @simFlush()
  %26 = load i32, i32* %2, align 4
  %27 = add i32 %26, 1
  store i32 %27, i32* %2, align 4
  br label %"47"

"49":                                             ; preds = %"47"
  ret void
}
