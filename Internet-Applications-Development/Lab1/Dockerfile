FROM nginx:1.15-alpine

RUN apk add --no-cache curl \
      php7 php7-fpm php7-phar php7-zip php7-session \
      php7-xml php7-simplexml php7-xmlwriter php7-dom php7-json \
      php7-tokenizer php7-mbstring php7-iconv php7-openssl php7-gd \
 && curl http://getcomposer.org/installer -o composer-setup.php \
 && php7 composer-setup.php --install-dir=/usr/bin --filename=composer \
 && rm composer-setup.php

RUN adduser -D dev

USER dev

WORKDIR /src
