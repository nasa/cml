test.print_tiv_bias()

print( "***** attempt bias without data loaded *****")
test.tiv_bias.bias_data( 1, 1, 3 );
test.print_tiv_bias()

print( "***** attempt scale without data loaded *****")
test.tiv_bias.scale_data( 1.5, 1, 3 );
test.print_tiv_bias()

print( "***** attempt bias with reversed start and stop index *****")
print( "***** should also fail monotonicity check *****")
test.load_bias_data();
test.tiv_bias.bias_data( 1, 3, 1 );
test.print_tiv_bias()

print( "***** attempt scale with reversed start and stop index *****")
print( "***** should also fail monotonicity check *****")
test.tiv_bias.scale_data( 2, 3, 1 );
test.print_tiv_bias()

print( "***** attempt valid scale *****")
print( "*** should get [2, 4.2, 6.3, 8.4, 9] ******")
test.tiv_bias.scale_data( 1.05, 1, 3 );
test.print_tiv_bias()

trick.stop(0)
