abort()
{
  MSG=$1
  echo
  echo "ERROR: $MSG"
  echo
  echo "abort."
  exit 1
}

separator()
{
  echo
  echo "==============================================================================="
  echo
}
