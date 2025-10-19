# #!/bin/bash
# # Запускаем программу в фоне
# time ./build/main -t $1 < test_input.txt &
# PID=$!

# echo "=== Мониторинг потоков для PID $PID ==="
# # Показываем количество потоков в реальном времени
# while kill -0 $PID 2>/dev/null; do
#     THREADS=$(ps -T -p $PID | wc -l)
#     echo "Активных потоков: $((THREADS - 1))"
#     sleep 0.1
# done

# wait $PID
# echo "Программа завершена"


#!/bin/bash
time ./build/main -t $1 < ./tests/test_input.txt &
PID=$!

MAX_THREADS=0
echo "=== Мониторинг PID $PID (опрос каждые 0.01с) ==="
while kill -0 $PID 2>/dev/null; do
    THREADS=$(ps -T -p $PID 2>/dev/null | tail -n +2 | wc -l)
    if [ $THREADS -gt $MAX_THREADS ]; then
        MAX_THREADS=$THREADS
    fi
    echo -ne "\rАктивных потоков: $THREADS | Максимум: $MAX_THREADS"
    sleep 0.0001  # Опрашиваем в 5 раз чаще
done

wait $PID
echo -e "\n=== Программа завершена. Максимум потоков (по ps): $MAX_THREADS ==="