require File.expand_path('../../../spec_helper', __FILE__)
require 'matrix'

describe "Matrix#coerce" do
  it "needs to be reviewed for spec completeness"

  ruby_bug "revision 24952", "1.8.7" do
    it "allows the division of fixnum by a Matrix " do
      (1/Matrix[[0,1],[-1,0]]).should == Matrix[[0,-1],[1,0]]
    end
  end
end
