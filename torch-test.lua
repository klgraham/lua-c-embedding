torch = require('torch')

--[[ 
  This function returns a matrix representation of an element of the group SO(2),
  the 2D rotation group.   
--]]
function rotation(theta)
  m = torch.Tensor(2,2)
  m[1][1] = torch.cos(theta)
  m[2][1] = torch.sin(theta)
  m[1][2] = -m[2][1]
  m[2][2] = m[1][1]
  return m
end

print(rotation(thetaInRadians))
