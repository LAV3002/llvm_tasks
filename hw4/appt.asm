app
    alloca x3 2500
    alloca x4 2500
    movi x5 0
app_loop_x
    movi x6 0
app_loop_y
    muli x7 x6 50
    add x7 x7 x5
    seti x4 x7 0
    call simRand
    remi x8 x0 5
    bnei x8 0 app_land
b_0
    seti x4 x7 1
app_land
    binci x6 50 app_loop_y
b_1
    binci x5 50 app_loop_x
b_2
    movi x5 0
    mov x0 x3
    mov x1 x4
app_loop_i
    b calc_gen
calc_gen_ret
    b draw_gen
draw_gen_ret
    call simFlush
    mov x2 x1
    mov x1 x0
    mov x0 x2
    binci x5 3 app_loop_i
b_3
    exit

draw_shape
    mov x13 x0
    mov x14 x1
    mov x9 x0
    mov x10 x1
    muli x0 x9 10
    addi x9 x9 1
    muli x11 x9 10
    addi x1 x1 1
    muli x12 x1 10
draw_shape_loop_i
    muli x1 x10 10
draw_shape_loop_j
    call simPutPixel
    addi x1 x1 1
    ble x1 x12 draw_shape_loop_j
b_4
    addi x0 x0 1
    ble x0 x11 draw_shape_loop_i
b_5
    mov x0 x13
    mov x1 x14
    b draw_shape_ret

draw_gen
    mov x8 x0
    movi x0 0
    movi x1 0
draw_gen_loop_y
    movi x0 0
draw_gen_loop_x
    mov x7 x1
    muli x7 x7 50
    add x7 x7 x0
    get x7 x8 x7
    muli x7 x7 65280
    addi x7 x7 4278190080
    mov x2 x7
    b draw_shape
draw_shape_ret
    addi x0 x0 1
    blei x0 50 draw_gen_loop_x
b_6
    addi x1 x1 1
    blei x1 50 draw_gen_loop_y
b_7
    b draw_gen_ret

calc_gen
    mov x10 x3
    mov x11 x4
    mov x12 x5
    mov x13 x6
    mov x8 x0
    mov x9 x1
    movi x0 0
    movi x1 0
    movi x2 0
    movi x3 0
    movi x4 0
calc_gen_loop_y
    movi x0 0
calc_gen_loop_x
    movi x4 0
    movi x2 -1
calc_gen_loop_dx
    movi x3 -1
calc_gen_loop_dy
    add x5 x1 x3
    addrmi x5 x5 50
    muli x5 x5 50
    add x6 x0 x2
    addrmi x6 x6 50
    add x5 x5 x6
    get x7 x9 x5
    add x4 x4 x7
    binci x3 2 calc_gen_loop_dy
b_8
    binci x2 2 calc_gen_loop_dx
b_9
    muli x2 x1 50
    add x2 x2 x0
    get x5 x9 x2
    sub x4 x4 x5
    seti x8 x2 0
    get x3 x9 x2
    bnei x3 1 calc_gen_land
b_10
    bei x4 2 calc_gen_tail_d_cycle
b_11
    bei x4 3 calc_gen_tail_d_cycle
b_12
    b calc_gen_end_d_cycle
calc_gen_land
    bei x4 3 calc_gen_tail_d_cycle
b_13
    b calc_gen_end_d_cycle
calc_gen_tail_d_cycle
    seti x8 x2 1
calc_gen_end_d_cycle
    binci x0 50 calc_gen_loop_x
b_14
    binci x1 50 calc_gen_loop_y
b_15
    mov x3 x10
    mov x4 x11
    mov x5 x12
    mov x6 x13
    b calc_gen_ret
