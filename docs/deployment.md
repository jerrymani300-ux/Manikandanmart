# ManiMart Deployment Guide

## 1. Docker Deployment (Recommended)

To deploy ManiMart using Docker Compose with PostgreSQL:

```bash
docker-compose up -d --build
```

The application will be accessible at `http://localhost:8080`.

---

## 2. Linux Production Deployment (systemd & Nginx)

### Build Application
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF ..
cmake --build . -j$(nproc)
```

### systemd Service (`/etc/systemd/system/manimart.service`)
```ini
[Unit]
Description=ManiMart E-Commerce Marketplace Server
After=network.target postgresql.service

[Service]
Type=simple
User=www-data
WorkingDirectory=/var/www/manimart
ExecStart=/var/www/manimart/manimart
Restart=always
RestartSec=5
EnvironmentFile=/var/www/manimart/.env

[Install]
WantedBy=multi-user.target
```

### Nginx Reverse Proxy (`/etc/nginx/sites-available/manimart`)
```nginx
server {
    listen 80;
    server_name marketplace.example.com;

    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```
