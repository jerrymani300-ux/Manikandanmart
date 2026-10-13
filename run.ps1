# ManiMart One-Click Launcher for PowerShell
Set-Location "C:\Users\LENOVO\Documents\mani mart"
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:PATH

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  ManiMart - Full-Stack Multi-Seller E-Commerce Marketplace" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "[1/2] Opening frontend in default browser..." -ForegroundColor Yellow
Start-Process "C:\Users\LENOVO\Documents\mani mart\frontend\index.html"
Write-Host "[2/2] Launching backend server on http://127.0.0.1:8080..." -ForegroundColor Yellow
Write-Host ""
Write-Host "Press Ctrl+C to stop the backend server anytime." -ForegroundColor Gray
Write-Host ""

& "C:\Users\LENOVO\Documents\mani mart\build_test\manimart.exe"
