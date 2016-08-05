function factorial(n)
  local result = 1
  for i = 1, n do
    result = result * i
  end
  return result
end

-- computes $\sum_{i=m}^N i$
function sum(m)
  local result = 0
  for i = m, N do
    result = result + i
  end
  return result
end
