; ModuleID = 'app.c'
source_filename = "app.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @draw_shape(i32 noundef %0, i32 noundef %1, i32 noundef %2) #0 {
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  store i32 %2, i32* %6, align 4
  store i32 0, i32* %7, align 4
  store i32 0, i32* %8, align 4
  %9 = load i32, i32* %4, align 4
  %10 = mul nsw i32 %9, 10
  store i32 %10, i32* %7, align 4
  br label %11

11:                                               ; preds = %34, %3
  %12 = load i32, i32* %7, align 4
  %13 = load i32, i32* %4, align 4
  %14 = add nsw i32 %13, 1
  %15 = mul nsw i32 %14, 10
  %16 = icmp ult i32 %12, %15
  br i1 %16, label %17, label %37

17:                                               ; preds = %11
  %18 = load i32, i32* %5, align 4
  %19 = mul nsw i32 %18, 10
  store i32 %19, i32* %8, align 4
  br label %20

20:                                               ; preds = %30, %17
  %21 = load i32, i32* %8, align 4
  %22 = load i32, i32* %5, align 4
  %23 = add nsw i32 %22, 1
  %24 = mul nsw i32 %23, 10
  %25 = icmp ult i32 %21, %24
  br i1 %25, label %26, label %33

26:                                               ; preds = %20
  %27 = load i32, i32* %7, align 4
  %28 = load i32, i32* %8, align 4
  %29 = load i32, i32* %6, align 4
  call void @simPutPixel(i32 noundef %27, i32 noundef %28, i32 noundef %29)
  br label %30

30:                                               ; preds = %26
  %31 = load i32, i32* %8, align 4
  %32 = add i32 %31, 1
  store i32 %32, i32* %8, align 4
  br label %20, !llvm.loop !6

33:                                               ; preds = %20
  br label %34

34:                                               ; preds = %33
  %35 = load i32, i32* %7, align 4
  %36 = add i32 %35, 1
  store i32 %36, i32* %7, align 4
  br label %11, !llvm.loop !8

37:                                               ; preds = %11
  ret void
}

declare void @simPutPixel(i32 noundef, i32 noundef, i32 noundef) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @drawGen(i32* noundef %0) #0 {
  %2 = alloca i32*, align 8
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32* %0, i32** %2, align 8
  store i32 0, i32* %3, align 4
  store i32 0, i32* %4, align 4
  store i32 0, i32* %4, align 4
  br label %5

5:                                                ; preds = %29, %1
  %6 = load i32, i32* %4, align 4
  %7 = icmp ult i32 %6, 50
  br i1 %7, label %8, label %32

8:                                                ; preds = %5
  store i32 0, i32* %3, align 4
  br label %9

9:                                                ; preds = %25, %8
  %10 = load i32, i32* %3, align 4
  %11 = icmp ult i32 %10, 50
  br i1 %11, label %12, label %28

12:                                               ; preds = %9
  %13 = load i32, i32* %3, align 4
  %14 = load i32, i32* %4, align 4
  %15 = load i32*, i32** %2, align 8
  %16 = load i32, i32* %4, align 4
  %17 = mul i32 %16, 50
  %18 = load i32, i32* %3, align 4
  %19 = add i32 %17, %18
  %20 = zext i32 %19 to i64
  %21 = getelementptr inbounds i32, i32* %15, i64 %20
  %22 = load i32, i32* %21, align 4
  %23 = mul i32 65280, %22
  %24 = add i32 -16777216, %23
  call void @draw_shape(i32 noundef %13, i32 noundef %14, i32 noundef %24)
  br label %25

25:                                               ; preds = %12
  %26 = load i32, i32* %3, align 4
  %27 = add i32 %26, 1
  store i32 %27, i32* %3, align 4
  br label %9, !llvm.loop !9

28:                                               ; preds = %9
  br label %29

29:                                               ; preds = %28
  %30 = load i32, i32* %4, align 4
  %31 = add i32 %30, 1
  store i32 %31, i32* %4, align 4
  br label %5, !llvm.loop !10

32:                                               ; preds = %5
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @calcGen(i32* noundef %0, i32* noundef %1) #0 {
  %3 = alloca i32*, align 8
  %4 = alloca i32*, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  store i32* %0, i32** %3, align 8
  store i32* %1, i32** %4, align 8
  store i32 0, i32* %5, align 4
  store i32 0, i32* %6, align 4
  store i32 0, i32* %7, align 4
  store i32 0, i32* %8, align 4
  store i32 0, i32* %9, align 4
  store i32 0, i32* %6, align 4
  br label %10

10:                                               ; preds = %119, %2
  %11 = load i32, i32* %6, align 4
  %12 = icmp slt i32 %11, 50
  br i1 %12, label %13, label %122

13:                                               ; preds = %10
  store i32 0, i32* %5, align 4
  br label %14

14:                                               ; preds = %115, %13
  %15 = load i32, i32* %5, align 4
  %16 = icmp slt i32 %15, 50
  br i1 %16, label %17, label %118

17:                                               ; preds = %14
  store i32 0, i32* %9, align 4
  store i32 -1, i32* %7, align 4
  br label %18

18:                                               ; preds = %48, %17
  %19 = load i32, i32* %7, align 4
  %20 = icmp slt i32 %19, 2
  br i1 %20, label %21, label %51

21:                                               ; preds = %18
  store i32 -1, i32* %8, align 4
  br label %22

22:                                               ; preds = %44, %21
  %23 = load i32, i32* %8, align 4
  %24 = icmp slt i32 %23, 2
  br i1 %24, label %25, label %47

25:                                               ; preds = %22
  %26 = load i32*, i32** %4, align 8
  %27 = load i32, i32* %6, align 4
  %28 = load i32, i32* %8, align 4
  %29 = add nsw i32 %27, %28
  %30 = add nsw i32 %29, 50
  %31 = srem i32 %30, 50
  %32 = mul nsw i32 %31, 50
  %33 = load i32, i32* %5, align 4
  %34 = load i32, i32* %7, align 4
  %35 = add nsw i32 %33, %34
  %36 = add nsw i32 %35, 50
  %37 = srem i32 %36, 50
  %38 = add nsw i32 %32, %37
  %39 = sext i32 %38 to i64
  %40 = getelementptr inbounds i32, i32* %26, i64 %39
  %41 = load i32, i32* %40, align 4
  %42 = load i32, i32* %9, align 4
  %43 = add i32 %42, %41
  store i32 %43, i32* %9, align 4
  br label %44

44:                                               ; preds = %25
  %45 = load i32, i32* %8, align 4
  %46 = add nsw i32 %45, 1
  store i32 %46, i32* %8, align 4
  br label %22, !llvm.loop !11

47:                                               ; preds = %22
  br label %48

48:                                               ; preds = %47
  %49 = load i32, i32* %7, align 4
  %50 = add nsw i32 %49, 1
  store i32 %50, i32* %7, align 4
  br label %18, !llvm.loop !12

51:                                               ; preds = %18
  %52 = load i32*, i32** %4, align 8
  %53 = load i32, i32* %6, align 4
  %54 = mul nsw i32 %53, 50
  %55 = load i32, i32* %5, align 4
  %56 = add nsw i32 %54, %55
  %57 = sext i32 %56 to i64
  %58 = getelementptr inbounds i32, i32* %52, i64 %57
  %59 = load i32, i32* %58, align 4
  %60 = load i32, i32* %9, align 4
  %61 = sub i32 %60, %59
  store i32 %61, i32* %9, align 4
  %62 = load i32*, i32** %4, align 8
  %63 = load i32, i32* %6, align 4
  %64 = mul nsw i32 %63, 50
  %65 = load i32, i32* %5, align 4
  %66 = add nsw i32 %64, %65
  %67 = sext i32 %66 to i64
  %68 = getelementptr inbounds i32, i32* %62, i64 %67
  %69 = load i32, i32* %68, align 4
  %70 = icmp ne i32 %69, 0
  br i1 %70, label %71, label %94

71:                                               ; preds = %51
  %72 = load i32, i32* %9, align 4
  %73 = icmp eq i32 %72, 2
  br i1 %73, label %77, label %74

74:                                               ; preds = %71
  %75 = load i32, i32* %9, align 4
  %76 = icmp eq i32 %75, 3
  br i1 %76, label %77, label %85

77:                                               ; preds = %74, %71
  %78 = load i32*, i32** %3, align 8
  %79 = load i32, i32* %6, align 4
  %80 = mul nsw i32 %79, 50
  %81 = load i32, i32* %5, align 4
  %82 = add nsw i32 %80, %81
  %83 = sext i32 %82 to i64
  %84 = getelementptr inbounds i32, i32* %78, i64 %83
  store i32 1, i32* %84, align 4
  br label %93

85:                                               ; preds = %74
  %86 = load i32*, i32** %3, align 8
  %87 = load i32, i32* %6, align 4
  %88 = mul nsw i32 %87, 50
  %89 = load i32, i32* %5, align 4
  %90 = add nsw i32 %88, %89
  %91 = sext i32 %90 to i64
  %92 = getelementptr inbounds i32, i32* %86, i64 %91
  store i32 0, i32* %92, align 4
  br label %93

93:                                               ; preds = %85, %77
  br label %114

94:                                               ; preds = %51
  %95 = load i32, i32* %9, align 4
  %96 = icmp eq i32 %95, 3
  br i1 %96, label %97, label %105

97:                                               ; preds = %94
  %98 = load i32*, i32** %3, align 8
  %99 = load i32, i32* %6, align 4
  %100 = mul nsw i32 %99, 50
  %101 = load i32, i32* %5, align 4
  %102 = add nsw i32 %100, %101
  %103 = sext i32 %102 to i64
  %104 = getelementptr inbounds i32, i32* %98, i64 %103
  store i32 1, i32* %104, align 4
  br label %113

105:                                              ; preds = %94
  %106 = load i32*, i32** %3, align 8
  %107 = load i32, i32* %6, align 4
  %108 = mul nsw i32 %107, 50
  %109 = load i32, i32* %5, align 4
  %110 = add nsw i32 %108, %109
  %111 = sext i32 %110 to i64
  %112 = getelementptr inbounds i32, i32* %106, i64 %111
  store i32 0, i32* %112, align 4
  br label %113

113:                                              ; preds = %105, %97
  br label %114

114:                                              ; preds = %113, %93
  br label %115

115:                                              ; preds = %114
  %116 = load i32, i32* %5, align 4
  %117 = add nsw i32 %116, 1
  store i32 %117, i32* %5, align 4
  br label %14, !llvm.loop !13

118:                                              ; preds = %14
  br label %119

119:                                              ; preds = %118
  %120 = load i32, i32* %6, align 4
  %121 = add nsw i32 %120, 1
  store i32 %121, i32* %6, align 4
  br label %10, !llvm.loop !14

122:                                              ; preds = %10
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @app() #0 {
  %1 = alloca [2500 x i32], align 16
  %2 = alloca [2500 x i32], align 16
  %3 = alloca i32*, align 8
  %4 = alloca i32*, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32*, align 8
  %9 = getelementptr inbounds [2500 x i32], [2500 x i32]* %1, i64 0, i64 0
  store i32* %9, i32** %3, align 8
  %10 = getelementptr inbounds [2500 x i32], [2500 x i32]* %2, i64 0, i64 0
  store i32* %10, i32** %4, align 8
  store i32 0, i32* %5, align 4
  br label %11

11:                                               ; preds = %34, %0
  %12 = load i32, i32* %5, align 4
  %13 = icmp slt i32 %12, 50
  br i1 %13, label %14, label %37

14:                                               ; preds = %11
  store i32 0, i32* %6, align 4
  br label %15

15:                                               ; preds = %30, %14
  %16 = load i32, i32* %6, align 4
  %17 = icmp slt i32 %16, 50
  br i1 %17, label %18, label %33

18:                                               ; preds = %15
  %19 = call i32 (...) @simRand()
  %20 = srem i32 %19, 6
  %21 = icmp eq i32 %20, 0
  %22 = zext i1 %21 to i32
  %23 = load i32*, i32** %4, align 8
  %24 = load i32, i32* %6, align 4
  %25 = mul nsw i32 %24, 50
  %26 = load i32, i32* %5, align 4
  %27 = add nsw i32 %25, %26
  %28 = sext i32 %27 to i64
  %29 = getelementptr inbounds i32, i32* %23, i64 %28
  store i32 %22, i32* %29, align 4
  br label %30

30:                                               ; preds = %18
  %31 = load i32, i32* %6, align 4
  %32 = add nsw i32 %31, 1
  store i32 %32, i32* %6, align 4
  br label %15, !llvm.loop !15

33:                                               ; preds = %15
  br label %34

34:                                               ; preds = %33
  %35 = load i32, i32* %5, align 4
  %36 = add nsw i32 %35, 1
  store i32 %36, i32* %5, align 4
  br label %11, !llvm.loop !16

37:                                               ; preds = %11
  call void (...) @simFlush()
  store i32 0, i32* %7, align 4
  store i32 0, i32* %7, align 4
  br label %38

38:                                               ; preds = %48, %37
  %39 = load i32, i32* %7, align 4
  %40 = icmp ult i32 %39, 2
  br i1 %40, label %41, label %51

41:                                               ; preds = %38
  %42 = load i32*, i32** %3, align 8
  %43 = load i32*, i32** %4, align 8
  call void @calcGen(i32* noundef %42, i32* noundef %43)
  %44 = load i32*, i32** %3, align 8
  call void @drawGen(i32* noundef %44)
  %45 = load i32*, i32** %4, align 8
  store i32* %45, i32** %8, align 8
  %46 = load i32*, i32** %3, align 8
  store i32* %46, i32** %4, align 8
  %47 = load i32*, i32** %8, align 8
  store i32* %47, i32** %3, align 8
  call void (...) @simFlush()
  br label %48

48:                                               ; preds = %41
  %49 = load i32, i32* %7, align 4
  %50 = add i32 %49, 1
  store i32 %50, i32* %7, align 4
  br label %38, !llvm.loop !17

51:                                               ; preds = %38
  ret void
}

declare i32 @simRand(...) #1

declare void @simFlush(...) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
!12 = distinct !{!12, !7}
!13 = distinct !{!13, !7}
!14 = distinct !{!14, !7}
!15 = distinct !{!15, !7}
!16 = distinct !{!16, !7}
!17 = distinct !{!17, !7}
